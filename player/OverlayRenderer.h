#pragma once

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

#include <EGL/egl.h>
#include <GLES/gl.h>

typedef struct
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
} VERTEX_3D;

typedef struct
{
	VERTEX_3D v1;
	VERTEX_3D v2;
	VERTEX_3D v3;
} TRIANGLE_3D;

class OverlayRenderer {
public:
	//TODO - unsure what these do :S
  OverlayRenderer(const OverlayRenderer&) = delete;
  OverlayRenderer& operator=(const OverlayRenderer&) = delete;
  //create renderer
  OverlayRenderer(int level);
  //destroy renderer
  ~OverlayRenderer();

  //show the next line(s) of subtitles
  void show_next() {
	// draw
    draw();
    //show on screen
    swap_buffers();
  }
  VERTEX_3D Vertex3DMake(float x, float y, float z);
  TRIANGLE_3D Triangle3DMake(VERTEX_3D v1, VERTEX_3D v2, VERTEX_3D v3);
private:

  //destroy something? TODO
  void destroy();
  //setup opengles
  void initialize_gles();
  //setup egl
  void initialize_egl();
  //destroy egl instance
  void destroy_egl();
  //initialise window to draw to
  void initialize_window(int layer);
  //destroy render window
  void destroy_window();
  //clear screen
  void clear();
  //draw to screen
  void draw();
  //switch display buffers
  void swap_buffers();

  //is renderer prepared to draw something
  bool prepared_;
  //display handles
  DISPMANX_ELEMENT_HANDLE_T dispman_element_;
  DISPMANX_DISPLAY_HANDLE_T dispman_display_;
  //screen dimensions
  uint32_t screen_width_;
  uint32_t screen_height_;
  //egl handles
  EGLDisplay display_;
  EGLContext context_;
  EGLSurface surface_;
};


