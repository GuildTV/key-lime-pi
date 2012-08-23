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

#include "ColourTexture.h"

#include "OverlayRenderer.h"

ColourTexture::ColourTexture(OverlayRenderer* render, TextureRender* parent): TextureRender(render, parent) {
    const char *vShaderStr =
      "attribute vec4 a_position;   \n"
      "attribute vec2 a_texCoord;   \n"
      "varying vec2 v_texCoord;     \n"
      "void main()                  \n"
      "{                            \n"
      "   gl_Position = a_position; \n"
      "   v_texCoord = a_texCoord;  \n"
      "}                            \n";

    const char *fShaderStr =
      "precision mediump float;                            \n"
      "varying vec2 v_texCoord;                            \n"
      "uniform sampler2D s_texture;                        \n"
      "uniform float red;                                  \n"
      "uniform float green;                                \n"
      "uniform float blue;                                 \n"
      "uniform float alpha;                                \n"
      "void main()                                         \n"
      "{                                                   \n"
      "  gl_FragColor = vec4(red * texture2D( s_texture, v_texCoord ).r, green * texture2D( s_texture, v_texCoord ).g, blue * texture2D( s_texture, v_texCoord ).b, alpha * texture2D( s_texture, v_texCoord ).a);\n"
      "}                                            \n";

    Setup(vShaderStr, fShaderStr);

    positionLoc = glGetAttribLocation (programObject, "a_position");
    texCoordLoc = glGetAttribLocation (programObject, "a_texCoord");
    samplerLoc = glGetUniformLocation(programObject, "s_texture");
    redLoc = glGetUniformLocation(programObject, "red");
    greenLoc = glGetUniformLocation(programObject, "green");
    blueLoc = glGetUniformLocation(programObject, "blue");
    alphaLoc = glGetUniformLocation(programObject, "alpha");

    red = 1.0f;
    green = 1.0f;
    blue = 1.0f;
    alpha = 1.0f;
}

void ColourTexture::SetColour(float r, float g, float b, float a){
    red = r;
    green = g;
    blue = b;
    alpha = a;
}

void ColourTexture::Render() {
    glUniform1f(redLoc, red);
    glUniform1f(greenLoc, green);
    glUniform1f(blueLoc, blue);
    glUniform1f(alphaLoc, alpha);

    getRenderer()->RenderTexture(getParent()->getTexture());
}
