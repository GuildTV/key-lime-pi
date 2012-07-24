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
#include <string.h>
#include <vector>

#ifdef RENDERTEST
#include  <X11/Xlib.h>
#include  <X11/Xatom.h>
#include  <X11/Xutil.h>
#define PNG_SKIP_SETJMP_CHECK
#include <png.h>
#define TEXTURE_LOAD_ERROR -1
#endif

#include "render/Freetype.h"

#include "logger.h"

enum Position {NONE, LEFT, CENTER, RIGHT};

class OverlayRenderer {
public:
    OverlayRenderer(std::string file);
    void Draw();
    void PreDraw();
    void Run();

    GLuint CreateSimpleTexture2D();
    bool LoadOverlayText();
    void DrawTimeStamp();
    void Stop();

    GLint getWidth() {return width;};
    GLint getHeight() {return height;};
    GLuint getPosLoc() {return positionLoc;};
    GLuint getTexLoc() {return texCoordLoc;};
    GLuint getSamLoc() {return samplerLoc;};

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
    GLuint loadTexture(const string filename, int &width, int &height);
    void LoadBG(string filename);
    GLuint bgTexture;
#endif

    std::string filename;

    GLint positionLoc;
    GLint texCoordLoc;
    GLint samplerLoc;

    Freetype *ft;
    TextChar overlayText[MAXCHARVALUE+1];

    long currentRefresh;
    Position timecodePosition;

    bool running;
};


