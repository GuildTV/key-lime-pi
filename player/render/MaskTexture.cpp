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

#include "MaskTexture.h"

MaskTexture::MaskTexture(OverlayRenderer* render, TextureRender* parent): TextureRender(render, parent){
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
      "    gl_FragColor = texture2D(s_texture, v_texCoord);\n"
      "}                                                   \n";

    Setup(vShaderStr, fShaderStr);

    positionLoc = glGetAttribLocation (programObject, "a_position");
    texCoordLoc = glGetAttribLocation (programObject, "a_texCoord");
    samplerLoc = glGetUniformLocation(programObject, "s_texture");

    points = new queue<MaskPoint>;
    prevPoint = new MaskPoint(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f);//create fullscreen (no crop)
}

void MaskTexture::addPoint(MaskPoint *p){
    points->push(*p);

    if(prevPoint->fieldNum == -1)
        prevPoint = p;
}

void MaskTexture::Render(int field) {

   GLfloat vVertices[] = { prevPoint->tl->x,  prevPoint->tl->y, 0.0f,  // Position 0
                             prevPoint->tl->x/2.0f+0.5f,  prevPoint->tl->x/2.0f+0.5f,        // TexCoord 0
                             prevPoint->bl->x,  prevPoint->bl->y, 0.0f,  // Position 1
                             prevPoint->bl->x/2.0f+0.5f,  prevPoint->bl->x/2.0f+0.5f,        // TexCoord 1
                             prevPoint->br->x,  prevPoint->br->y, 0.0f,  // Position 2
                             prevPoint->br->x/2.0f+0.5f,  prevPoint->br->x/2.0f+0.5f,        // TexCoord 2
                             prevPoint->tr->x,  prevPoint->tr->y, 0.0f,  // Position 3
                             prevPoint->tr->x/2.0f+0.5f,  prevPoint->tr->x/2.0f+0.5f         // TexCoord 3
                          };

    if(!points->empty()){
        MaskPoint next = points->front();
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

        if(deltaF > 0){
            vVertices[0] = prevPoint->tl->x + (next.tl->x - prevPoint->tl->x)*ratioF;
            vVertices[1] = prevPoint->tl->y + (next.tl->y - prevPoint->tl->y)*ratioF;

            vVertices[3] = (prevPoint->tl->x + (next.tl->x - prevPoint->tl->x)*ratioF)/2.0f+0.5f;
            vVertices[4] = (prevPoint->tl->y + (next.tl->y - prevPoint->tl->y)*ratioF)/2.0f+0.5f;

            vVertices[5] = prevPoint->bl->x + (next.bl->x - prevPoint->bl->x)*ratioF;
            vVertices[6] = prevPoint->bl->y + (next.bl->y - prevPoint->bl->y)*ratioF;

            vVertices[8] = (prevPoint->bl->x + (next.bl->x - prevPoint->bl->x)*ratioF)/2.0f+0.5f;
            vVertices[9] = (prevPoint->bl->y + (next.bl->y - prevPoint->bl->y)*ratioF)/2.0f+0.5f;

            vVertices[10] = prevPoint->br->x + (next.br->x - prevPoint->br->x)*ratioF;
            vVertices[11] = prevPoint->br->y + (next.br->y - prevPoint->br->y)*ratioF;

            vVertices[13] = (prevPoint->br->x + (next.br->x - prevPoint->br->x)*ratioF)/2.0f+0.5f;
            vVertices[14] = (prevPoint->br->y + (next.br->y - prevPoint->br->y)*ratioF)/2.0f+0.5f;

            vVertices[15] = prevPoint->tr->x + (next.tr->x - prevPoint->tr->x)*ratioF;
            vVertices[16] = prevPoint->tr->y + (next.tr->y - prevPoint->tr->y)*ratioF;

            vVertices[18] = (prevPoint->tr->x + (next.tr->x - prevPoint->tr->x)*ratioF)/2.0f+0.5f;
            vVertices[19] = (prevPoint->tr->y + (next.tr->y - prevPoint->tr->y)*ratioF)/2.0f+0.5f;
        }

    }


        GLushort indices[] = { 0, 1, 2, 0, 2, 3};

        glVertexAttribPointer (positionLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), vVertices );
        glVertexAttribPointer (texCoordLoc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), &vVertices[3] );

        glEnableVertexAttribArray ( positionLoc );
        glEnableVertexAttribArray ( texCoordLoc );

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, getParent()->getTexture());
        glUniform1i ( samplerLoc, 0 );
        glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );
}

