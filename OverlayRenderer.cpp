/*
 *      Copyright (C) 2012 GuildTV
 *      http://www.guildtv.co.uk
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "OverlayRenderer.h"
#include "folly/ScopeGuard.h"
#include "enforce.h"

#include <cassert>

//destroy the renderer
OverlayRenderer::~OverlayRenderer() {
  destroy();
}

//create the renderer
OverlayRenderer::
OverlayRenderer(int layer):
//set no value
dispman_element_(),
dispman_display_(),
screen_width_(),
screen_height_(),
display_(),
context_(),
surface_() {
  try {
	  //initialise the window
    initialize_window(layer);
	//initialise egl
    initialize_egl();
	//initialise gles
	initialize_gles();

  } catch (...) {
    destroy();
    throw;
  }
}

//destroy
void OverlayRenderer::destroy() {
  destroy_egl();
  destroy_window();
}

//setup opengl es
void OverlayRenderer::initialize_gles() {
	//set transparent black bg
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//clear
	glClear( GL_COLOR_BUFFER_BIT );
	glClear( GL_DEPTH_BUFFER_BIT );
	//set shade model
	glShadeModel(GL_FLAT);

	glMatrixMode (GL_PROJECTION);
glLoadIdentity ();
//glOrtho (0, screen_width_, screen_height_, 0, 0, 1);
glMatrixMode (GL_MODELVIEW);
}
//setup window
void OverlayRenderer::initialize_window(int layer) {
	//check display has size
  ENFORCE(graphics_get_display_size(0 /* LCD */, &screen_width_, &screen_height_)
          >= 0);

//define dest size as a rectangle
  VC_RECT_T dst_rect;
  dst_rect.x = 0;
  dst_rect.y = 0;
  dst_rect.width = screen_width_;
  dst_rect.height = screen_height_;

//define src size as a rectangle
  VC_RECT_T src_rect;
  src_rect.x = 0;
  src_rect.y = 0;
  src_rect.width = screen_width_ << 16;
  src_rect.height = screen_height_ << 16;

//open display 0
  dispman_display_ = vc_dispmanx_display_open(0 /* LCD */);
  ENFORCE(dispman_display_);

  {
	  //start update display loop??
    auto dispman_update = vc_dispmanx_update_start(0);
    ENFORCE(dispman_update);
    SCOPE_EXIT {
      ENFORCE(!vc_dispmanx_update_submit_sync(dispman_update));
    };

	//create display element
    dispman_element_ =
        vc_dispmanx_element_add(dispman_update,
                                dispman_display_,
                                layer,
                                &dst_rect,
                                0 /*src*/,
                                &src_rect,
                                DISPMANX_PROTECTION_NONE,
                                0 /*alpha*/,
                                0 /*clamp*/,
                                (DISPMANX_TRANSFORM_T) 0 /*transform*/);
    ENFORCE(dispman_element_);
  }
}

//destroy the window TODO - no clue how
void OverlayRenderer::destroy_window() {
  if (dispman_element_) {
    auto dispman_update = vc_dispmanx_update_start(0);
    assert(dispman_update);

    if (dispman_update) {
      auto error = vc_dispmanx_element_remove(dispman_update, dispman_element_);
      assert(!error);

      error = vc_dispmanx_update_submit_sync(dispman_update);
      assert(!error);
    }

    dispman_element_ = {};
  }

  if (dispman_display_) {
    auto error = vc_dispmanx_display_close(dispman_display_);
    assert(!error);

    dispman_display_ = {};
  }
}

//initialise egl
void OverlayRenderer::initialize_egl() {
  // get an EGL display connection
  display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  ENFORCE(display_);

  // initialize the EGL display connection
  ENFORCE(eglInitialize(display_, NULL, NULL));

  // get an appropriate EGL frame buffer configuration
  static const EGLint attribute_list[] = {
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_NONE
  };
  EGLConfig config{};
  EGLint num_config{};

	//load config
  ENFORCE(eglChooseConfig(display_, attribute_list, &config, 1, &num_config));
  ENFORCE(num_config);

	//bing opengl api
  ENFORCE(eglBindAPI(EGL_OPENGL_ES_API));

//egl window
  static EGL_DISPMANX_WINDOW_T nativewindow;
  nativewindow.element = dispman_element_;
  nativewindow.width = screen_width_;
  nativewindow.height = screen_height_;

	 //create window surface
  surface_ = eglCreateWindowSurface(display_, config, &nativewindow, NULL);
  ENFORCE(surface_);

  // create an EGL rendering context
  context_ = eglCreateContext(display_, config, EGL_NO_CONTEXT, NULL);
  ENFORCE(context_);

//make it current
  auto result = eglMakeCurrent(display_, surface_, surface_, context_);
  assert(result);
}

//destroy egl
void OverlayRenderer::destroy_egl() {
  if (display_) {
	  //set null as current display
    auto result =
      eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    assert(result);

	//terminate display
    result = eglTerminate(display_);
    assert(result);

	//clear display stuff
    context_ = {};
    surface_ = {};
    display_ = {};
  }
}


//clear screen
void OverlayRenderer::clear() {
  //clear screen
  glClear(GL_COLOR_BUFFER_BIT);
  assert(!glGetError());
}

//draw
void OverlayRenderer::draw() {
  clear();

  static GLfloat rot = 0.5;

	VERTEX_3D vertex1 = Vertex3DMake(0.0f, 1.0f, 1.0f);
	VERTEX_3D vertex2 = Vertex3DMake(1.0f, 0.0f, 1.0f);
	VERTEX_3D vertex3 = Vertex3DMake(-1.0f, 0.0f, 1.0f);

	VERTEX_3D vertex4 = Vertex3DMake(0.0f, -1.0f, 1.0f);

	TRIANGLE_3D triangles[2];
	triangles[0] = Triangle3DMake(vertex1, vertex3, vertex2);
	triangles[1] = Triangle3DMake(vertex4, vertex2, vertex3);

	glRotatef(rot, 0.0, 0.0, 1.0);
	//glClearColor(0.7, 0.7 , 0.7, 1.0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnableClientState(GL_VERTEX_ARRAY);
	glColor4f(1.0, 0.0, 0.0, 1.0);
	glVertexPointer(3, GL_FLOAT, 0, &triangles);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableClientState(GL_VERTEX_ARRAY);
  //TODO - draw here :)
}

//show on screen
void OverlayRenderer::swap_buffers() {
  EGLBoolean result = eglSwapBuffers(display_, surface_);
  assert(result);
}

VERTEX_3D OverlayRenderer::Vertex3DMake(float x, float y, float z)
{
	VERTEX_3D ret;
	ret.x = (GLfloat)x;
	ret.y = (GLfloat)y;
	ret.z = (GLfloat)z;
	return ret;
}

TRIANGLE_3D OverlayRenderer::Triangle3DMake(VERTEX_3D v1, VERTEX_3D v2, VERTEX_3D v3)
{
	TRIANGLE_3D ret;
	ret.v1 = v1;
	ret.v2 = v2;
	ret.v3 = v3;
	return ret;
}
