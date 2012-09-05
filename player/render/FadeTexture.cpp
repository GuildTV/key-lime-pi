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

    points = new queue<FadePoint>;
    prevPoint = new FadePoint(-1, 1.0f);
}

void FadeTexture::addPoint(FadePoint *p){
    points->push(*p);

    if(prevPoint->fieldNum == -1)
        prevPoint = p;
}

void FadeTexture::Render(int field) {
    float currentAlpha = prevPoint->alpha;

    if(!points->empty()){
        FadePoint next = points->front();
        if(next.fieldNum <= field){
            prevPoint = &points->front();
            points->pop();

            Render(field);
            return;
        }

        int prevF = prevPoint->fieldNum;
        int nextF = next.fieldNum;
        int deltaF = nextF - prevF;
        int relF = field - prevF;
        float ratioF = ((float) relF)/((float) deltaF);

        if(deltaF <= 0){
            currentAlpha = prevPoint->alpha;
        } else {
            float baseA = prevPoint->alpha;
            float deltaA = next.alpha - baseA;

            currentAlpha = baseA + deltaA*ratioF;
        }
    }

    glUniform1f(alphaLoc, currentAlpha);

    getRenderer()->RenderTexture(getParent()->getTexture());
}
