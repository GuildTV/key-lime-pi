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
      "uniform vec4 colour;                                \n"
      "void main()                                         \n"
      "{                                                   \n"
      "  gl_FragColor = colour * texture2D( s_texture, v_texCoord );\n"
      "}                                            \n";

    Setup(vShaderStr, fShaderStr);

    positionLoc = glGetAttribLocation (programObject, "a_position");
    texCoordLoc = glGetAttribLocation (programObject, "a_texCoord");
    samplerLoc = glGetUniformLocation(programObject, "s_texture");
    colourLoc = glGetUniformLocation(programObject, "colour");

    colour[0] = 1.0f;
    colour[1] = 1.0f;
    colour[2] = 1.0f;
    colour[3] = 1.0f;
}

void ColourTexture::SetColour(float r, float g, float b, float a){
    colour[0] = r;
    colour[1] = g;
    colour[2] = b;
    colour[3] = a;
}

void ColourTexture::Render() {

    glUniform4fv(colourLoc, 1, colour);
    glEnableVertexAttribArray ( positionLoc );

    getRenderer()->RenderTexture(getParent()->getTexture());
}
