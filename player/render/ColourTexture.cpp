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
      "uniform vec4 colour1;                               \n"
      "uniform vec2 colour1Pos;                            \n"
      "uniform vec4 colour2;                               \n"
      "uniform vec2 colour2Pos;                            \n"
      "uniform float useCol2;                              \n"
      "vec4 xCol() {                                       \n"
      "    if(v_texCoord[0] < colour2Pos[0] && v_texCoord[0] < colour1Pos[0])\n"
      "      return colour1;                               \n"
      "    if(v_texCoord[0] > colour2Pos[0] && v_texCoord[0] > colour1Pos[0])\n"
      "      return colour2;                               \n"
      "    float abx = colour2Pos[0] - colour1Pos[0];      \n"
      "    float apx = colour1Pos[0] - v_texCoord[0];      \n"
      "    float r = abs(apx/abx);                         \n"
      "                                                    \n"
      "    return mix(colour1, colour2, r);                \n"
      " }                                                  \n"
      "vec4 yCol() {                                       \n"
      "    if(v_texCoord[1] < colour2Pos[1] && v_texCoord[1] < colour1Pos[1])\n"
      "      return colour1;                               \n"
      "    if(v_texCoord[1] > colour2Pos[1] && v_texCoord[1] > colour1Pos[1])\n"
      "      return colour2;                               \n"
      "    float aby = colour2Pos[1] - colour1Pos[1];      \n"
      "    float apy = colour1Pos[1] - v_texCoord[1];      \n"
      "    float r = abs(apy/aby);                         \n"
      "                                                    \n"
      "    return mix(colour1, colour2, r);                \n"
      " }                                                  \n"
      "void main() {                                       \n"
      " if(useCol2 <= 0.5) {                               \n"
      "    gl_FragColor = colour1 * texture2D( s_texture, v_texCoord );\n"
      "  } else {                                          \n"
      "                                                    \n"
      "    float dx = colour2Pos[0] - colour1Pos[0];       \n"
      "    float dy = colour2Pos[1] - colour1Pos[1];       \n"
      "    float len = dx+dy;                              \n"
      "    gl_FragColor = (xCol()*dx/len + yCol()*dy/len)*texture2D( s_texture, v_texCoord );   \n"
      "                                                    \n"
      "                                                    \n"
      "                                                    \n"
      "  }                                                 \n"
      "}                                                   \n";

    Setup(vShaderStr, fShaderStr);

    positionLoc = glGetAttribLocation (programObject, "a_position");
    texCoordLoc = glGetAttribLocation (programObject, "a_texCoord");
    samplerLoc = glGetUniformLocation(programObject, "s_texture");
    colour1Loc = glGetUniformLocation(programObject, "colour1");
    colour1PosLoc = glGetUniformLocation(programObject, "colour1Pos");
    colour2Loc = glGetUniformLocation(programObject, "colour2");
    colour2PosLoc = glGetUniformLocation(programObject, "colour2Pos");
    useCol2Loc = glGetUniformLocation(programObject, "useCol2");

    useColour2 = false;
}

void ColourTexture::SetColour1(float x, float y, float r, float g, float b, float a){
    colour1.position[0] = x;
    colour1.position[1] = y;

    colour1.colour[0] = r;
    colour1.colour[1] = g;
    colour1.colour[2] = b;
    colour1.colour[3] = a;
}
/*
void ColourTexture::SetColour2(float x, float y, float r, float g, float b, float a){
    colour2.position[0] = x;
    colour2.position[1] = y;

    colour2.colour[0] = r;
    colour2.colour[1] = g;
    colour2.colour[2] = b;
    colour2.colour[3] = a;

    useColour2 = true;
}
*/
void ColourTexture::Render() {
    glUniform4fv(colour1Loc, 1, colour1.colour);
    glUniform2fv(colour1PosLoc, 1, colour1.position);
    glUniform4fv(colour2Loc, 1, colour2.colour);
    glUniform2fv(colour2PosLoc, 1, colour2.position);
    glUniform1f(useCol2Loc, useColour2);

    getRenderer()->RenderTexture(getParent()->getTexture());
}
