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

/**
 * Render onto the screen via OpenGL ES2.0
**/

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
#endif

#include "render/Freetype.h"
#include "net/NetIO.h"

#include "render/EffectParser.h"

#include "logger.h"

//define the position of the timecode
enum Position {NONE, LEFT, CENTER, RIGHT};

class OverlayRenderer {
public:
    OverlayRenderer(NetIO *net);
    //setup the renderer to use specified script
    void Create(std::string file, Json::Value data);
    //draw frame
    void Draw();
    //draw the prevideo frame
    void PreDraw();
    //run the rendering loop
    void Run();

    //load the textures for the overlay text
    bool LoadOverlayText();
    //draw the timestamp to the screen
    void DrawTimeStamp();
    //stop the rendering
    void Stop();

    //get the screen dimensions
    GLint getWidth() {return width;};
    GLint getHeight() {return height;};
    //get shader values
    GLuint getPosLoc() {return positionLoc;};
    GLuint getTexLoc() {return texCoordLoc;};
    GLuint getSamLoc() {return samplerLoc;};

    //get handle to freetype
    Freetype* getFT() {return ft;};
    //get the default charset
    TextChar* getDefaultCharSet() {return overlayText;};

    //render a texture to the screen
    void RenderTexture(GLuint texture);

    //create a new program object
    GLuint CreateProgram(const char *vShaderStr, const char *fShaderStr, GLuint *programObject);

#ifdef LIMESLAVE
    void unBind();
#endif

protected:
    //create opengl window
    GLboolean esCreateWindow (const char* title);
    //create egl window
    EGLBoolean WinCreate(const char *title);
    //create egl context
    EGLBoolean CreateEGLContext();
    //setup renderer
    int Init();
    //load shaders
    GLuint LoadShader(GLenum type, const char *shaderSrc);

private:
    //opengl program object
    GLuint programObject;
    //window dimensions
    GLint width;
    GLint height;
    //egl handles
    EGLNativeWindowType hWnd;
    EGLDisplay eglDisplay;
    EGLContext eglContext;
    EGLSurface eglSurface;
#ifdef RENDERTEST
    //x11 display
    Display *x_display;
#endif

    //vector of render elements
    vector<TextureRender*> renderElms;

    //script filename
    std::string filename;

    //opengl shader handles
    GLint positionLoc;
    GLint texCoordLoc;
    GLint samplerLoc;

    //freetype renderer pointer
    Freetype *ft;
    //overlay text character array
    TextChar overlayText[MAXCHARVALUE+1];

    //current screen refresh count
    long currentRefresh;
    //timecode position on screen
    Position timecodePosition;

    //is loop running
    bool running;

    //network handler
    NetIO *netIO;
};
