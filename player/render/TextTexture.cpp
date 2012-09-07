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

#include "TextTexture.h"

#include "OverlayRenderer.h"

TextTexture::TextTexture(OverlayRenderer* render): TextureRender(render)  {
    defined = false;
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
      "void main()                                         \n"
      "{                                                   \n"
      "  gl_FragColor = vec4(1, 1, 1, texture2D( s_texture, v_texCoord ).a);\n"
      "}                                            \n";

    Setup(vShaderStr, fShaderStr);

    positionLoc = glGetAttribLocation(programObject, "a_position");
    texCoordLoc = glGetAttribLocation(programObject, "a_texCoord");
    samplerLoc = glGetUniformLocation(programObject, "s_texture");
}

void TextTexture::setText(const char* s, TextChar* chars, int x, int y, float xs, float ys){
    text = s;
    charset = chars;
    xPos = x;
    yPos = y;
    xScale = xs;
    yScale = ys;
    defined = true;

    SwitchTo();
}

void TextTexture::Render(int field) {
    if(defined)
        getRenderer()->getFT()->WriteString(text, charset, xPos, yPos, xScale, yScale);
}
