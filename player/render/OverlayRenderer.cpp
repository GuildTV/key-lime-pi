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

int OverlayRenderer::Create(std::string file) {
    filename = file;
    esCreateWindow ("Overlay Renderer");

    if (!Init ())
        return 1;

    return 0;
}

void OverlayRenderer::Draw () {
    GLfloat vVertices[] = {  0.0f,  0.5f, 0.0f,
                            -0.5f, -0.5f, 0.0f,
                             0.5f, -0.5f, 0.0f };

    // Set the viewport
    glViewport(0, 0, width, height);

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);


    glClearColor(0.0f, 1.0f, 1.0f, 0.5f);

    // Use the program object
    glUseProgram(programObject);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    eglSwapBuffers(eglDisplay, eglSurface);
}

void OverlayRenderer::Run() {
    Draw();
}

void OverlayRenderer::PreDraw() {
    //Draw();
}

GLboolean OverlayRenderer::esCreateWindow (const char* title) {
    if (!WinCreate (title)) {
        return GL_FALSE;
    }

    if (!CreateEGLContext ()) {
        return GL_FALSE;
    }

    return GL_TRUE;
}

int OverlayRenderer::Init () {
    const char *vShaderStr =
        "attribute vec4 vPosition;    \n"
        "void main()                  \n"
        "{                            \n"
        "   gl_Position = vPosition;  \n"
        "}                            \n";

    const char *fShaderStr =
        "precision mediump float;\n"\
        "void main()                                  \n"
        "{                                            \n"
        "  gl_FragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );\n"
        "}                                            \n";

    GLuint vertexShader;
    GLuint fragmentShader;
    GLint linked;

    // Load the vertex/fragment shaders
    vertexShader = LoadShader(GL_VERTEX_SHADER, vShaderStr);
    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);

    // Create the program object
    programObject = glCreateProgram();

    if (programObject == 0)
        return 0;

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    // Bind vPosition to attribute 0
    glBindAttribLocation(programObject, 0, "vPosition");

    // Link the program
    glLinkProgram(programObject);

    // Check the link status
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);

    if (!linked) {
        GLint infoLen = 0;

        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1) {
            printf ( "Error linking program:\n");
        }

        glDeleteProgram(programObject);
        return GL_FALSE;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    return GL_TRUE;
}

GLuint OverlayRenderer::LoadShader (GLenum type, const char *shaderSrc) {
    GLuint shader;
    GLint compiled;

    // Create the shader object
    shader = glCreateShader(type);

    if (shader == 0)
        return 0;

    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, NULL);

    // Compile the shader
    glCompileShader(shader);

    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        GLint infoLen = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen > 1) {
            printf ("Error compiling shader\n");
        }

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

#ifndef RENDERTEST
EGLBoolean OverlayRenderer::WinCreate(const char *title) {
   int32_t success = 0;

   static EGL_DISPMANX_WINDOW_T nativewindow;

   DISPMANX_ELEMENT_HANDLE_T dispman_element;
   DISPMANX_DISPLAY_HANDLE_T dispman_display;
   DISPMANX_UPDATE_HANDLE_T dispman_update;
   VC_RECT_T dst_rect;
   VC_RECT_T src_rect;


   uint32_t display_width;
   uint32_t display_height;

   // create an EGL window surface, passing context width/height
   success = graphics_get_display_size(0, &display_width, &display_height);
   if (success < 0) {
      return EGL_FALSE;
   }

   // You can hardcode the resolution here:
   display_width = 720;
   display_height = 576;

   dst_rect.x = 0;
   dst_rect.y = 0;
   dst_rect.width = display_width;
   dst_rect.height = display_height;

   src_rect.x = 0;
   src_rect.y = 0;
   src_rect.width = display_width << 16;
   src_rect.height = display_height << 16;

   dispman_display = vc_dispmanx_display_open(0);
   dispman_update = vc_dispmanx_update_start(0);

   dispman_element = vc_dispmanx_element_add (dispman_update, dispman_display, 1, &dst_rect, 0, &src_rect,
                                              DISPMANX_PROTECTION_NONE, 0, 0, (DISPMANX_TRANSFORM_T) 0);

   nativewindow.element = dispman_element;
   nativewindow.width = display_width;
   nativewindow.height = display_height;
   vc_dispmanx_update_submit_sync( dispman_update );

   hWnd = &nativewindow;
   width = display_width;
   height = display_height;

	return EGL_TRUE;
}
#else
EGLBoolean OverlayRenderer::WinCreate(const char *title)
{
    Window root;
    XSetWindowAttributes swa;
    XSetWindowAttributes  xattr;
    Atom wm_state;
    XWMHints hints;
    XEvent xev;
    Window win;

    /*
     * X11 native display initialization
     */

    width = 1024;
    height = 576;

    x_display = XOpenDisplay(NULL);
    if (x_display == NULL) {
        return EGL_FALSE;
    }

    root = DefaultRootWindow(x_display);

    swa.event_mask = ExposureMask | PointerMotionMask | KeyPressMask;
    win = XCreateWindow(x_display, root, 0, 0, width, height, 0, CopyFromParent, InputOutput, CopyFromParent, CWEventMask, &swa);

    xattr.override_redirect = 0;
    XChangeWindowAttributes (x_display, win, CWOverrideRedirect, &xattr);

    hints.input = 1;
    hints.flags = InputHint;
    XSetWMHints(x_display, win, &hints);

    // make the window visible on the screen
    XMapWindow (x_display, win);
    XStoreName (x_display, win, title);

    // get identifiers for the provided atom name strings
    wm_state = XInternAtom (x_display, "_NET_WM_STATE", 0);

    memset ( &xev, 0, sizeof(xev) );
    xev.type                 = ClientMessage;
    xev.xclient.window       = win;
    xev.xclient.message_type = wm_state;
    xev.xclient.format       = 32;
    xev.xclient.data.l[0]    = 1;
    xev.xclient.data.l[1]    = 0;
    XSendEvent (x_display, DefaultRootWindow (x_display), 0, SubstructureNotifyMask, &xev);

    hWnd = (EGLNativeWindowType) win;
    return EGL_TRUE;
}
#endif

OverlayRenderer::OverlayRenderer(){
#ifdef RENDERTEST
    x_display = NULL;
#endif
}

EGLBoolean OverlayRenderer::CreateEGLContext(){
#ifndef RENDERTEST
    EGLint attribList[] =
    {
        EGL_RED_SIZE,       8,
        EGL_GREEN_SIZE,     8,
        EGL_BLUE_SIZE,      8,
        EGL_ALPHA_SIZE,     8,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE
    };
#else
    EGLint attribList[] =
    {
        EGL_RED_SIZE,       5,
        EGL_GREEN_SIZE,     6,
        EGL_BLUE_SIZE,      5,
        EGL_ALPHA_SIZE,     (0 & 1) ? 8 : EGL_DONT_CARE,
        EGL_DEPTH_SIZE,     (0 & 2) ? 8 : EGL_DONT_CARE,
        EGL_STENCIL_SIZE,   (0 & 4) ? 8 : EGL_DONT_CARE,
        EGL_SAMPLE_BUFFERS, (0 & 8) ? 1 : 0,
        EGL_NONE
    };
#endif

    EGLint numConfigs;
    EGLConfig config;
#ifdef RENDERTEST
    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };
#else
    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
#endif

    // Get Display
#ifdef RENDERTEST
    eglDisplay = eglGetDisplay((EGLNativeDisplayType)x_display);
#else
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
#endif
    if (eglDisplay == EGL_NO_DISPLAY) {
        return EGL_FALSE;
    }

    // Initialize EGL
    if (!eglInitialize(eglDisplay, NULL, NULL)) {
        return EGL_FALSE;
    }

    // Get configs
    if (!eglGetConfigs(eglDisplay, NULL, 0, &numConfigs)) {
        return EGL_FALSE;
    }

    // Choose config
    if (!eglChooseConfig(eglDisplay, attribList, &config, 1, &numConfigs)) {
        return EGL_FALSE;
    }

    eglBindAPI(EGL_OPENGL_ES_API);

    // Create a surface
    eglSurface = eglCreateWindowSurface(eglDisplay, config, (EGLNativeWindowType)hWnd, NULL);
    if (eglSurface == EGL_NO_SURFACE) {
        return EGL_FALSE;
    }

    // Create a GL context
    eglContext = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, contextAttribs );
    if (eglContext == EGL_NO_CONTEXT) {
        return EGL_FALSE;
    }

    // Make the context current
    if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)) {
        return EGL_FALSE;
    }

    return EGL_TRUE;
}
