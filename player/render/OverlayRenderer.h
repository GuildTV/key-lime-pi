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
#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef RENDERTEST
#include  <X11/Xlib.h>
#include  <X11/Xatom.h>
#include  <X11/Xutil.h>
#define PNG_SKIP_SETJMP_CHECK
#include <png.h>
#define TEXTURE_LOAD_ERROR -1
#endif

#include "logger.h"

#define MAXCHARVALUE 127
#define DEFAULTCHARVALUE 45

enum Position {NONE, LEFT, CENTER, RIGHT};

struct TextChar {
    TextChar() :loaded(false), texture(-1){}
    GLuint texture;
    int width;
    int height;
    int advanceX;
    int advanceY;
    int bitTop;
    int bitLeft;
    bool loaded;
};

class OverlayRenderer {
public:
    int Create(std::string file);
    OverlayRenderer();
    void Draw();
    void PreDraw();
    void Run();
    bool LoadFreetype(string font_file);
    bool LoadFreetypeChar(int height, int value, TextChar *character);
    void CloseFreetype();
    void FreetypeToTexture(FT_GlyphSlot *slot, TextChar* character);
    GLuint CreateSimpleTexture2D();
    unsigned int pow2(unsigned int num);
    bool LoadFreetypeRange(string font_file, int height, int start, int end, TextChar *storage);
    bool LoadOverlayText();
    void WriteString(char * text, TextChar *charSet, int x, int y, float scaleX, float scaleY);
    void DrawTimeStamp();

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
    FT_Library library;
    FT_Face face;
    GLuint texture;
    GLint positionLoc;
    GLint texCoordLoc;
    GLint samplerLoc;

    TextChar overlayText[MAXCHARVALUE+1];

    long currentRefresh;
    Position timecodePosition;
};


