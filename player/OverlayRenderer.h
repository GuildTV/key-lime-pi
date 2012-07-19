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

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>
#include <string>

#ifdef RENDERTEST
#include  <X11/Xlib.h>
#include  <X11/Xatom.h>
#include  <X11/Xutil.h>
#endif

class OverlayRenderer {
public:
    int Create(std::string file);
    OverlayRenderer();
    void Draw();
    void PreDraw();
    void Run();

protected:
    GLboolean esCreateWindow (const char* title);
    EGLBoolean WinCreate(const char *title);
    EGLBoolean CreateEGLContext();
    int Init();
    GLuint LoadShader(GLenum type, const char *shaderSrc);

private:
    GLuint programObject;
    GLint width;
    GLint height;
    EGLNativeWindowType hWnd;
    EGLDisplay eglDisplay;
    EGLContext eglContext;
    EGLSurface eglSurface;
#ifdef RENDERTEST
    Display *x_display;
#endif

    std::string filename;
};


