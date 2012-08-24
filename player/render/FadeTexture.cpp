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

#include "FadeTexture.h"

#include "OverlayRenderer.h"

FadeTexture::FadeTexture(OverlayRenderer* render, TextureRender* parent): TextureRender(render, parent){
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
      "uniform float alpha;                               \n"
      "void main() {                                       \n"
      "    gl_FragColor = texture2D(s_texture, v_texCoord) * vec4(1, 1, 1, alpha);\n"
      "}                                                   \n";

    Setup(vShaderStr, fShaderStr);

    positionLoc = glGetAttribLocation (programObject, "a_position");
    texCoordLoc = glGetAttribLocation (programObject, "a_texCoord");
    samplerLoc = glGetUniformLocation(programObject, "s_texture");
    alphaLoc = glGetUniformLocation(programObject, "alpha");
}

void FadeTexture::Render() {
    float currentAlpha = 0.9f;
    glUniform1f(alphaLoc, currentAlpha);

    getRenderer()->RenderTexture(getParent()->getTexture());
}
