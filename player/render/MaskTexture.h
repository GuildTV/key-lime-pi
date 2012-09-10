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

#ifndef MASKTEXTURE_H
#define MASKTEXTURE_H

#include "render/TextureRender.h"
#include <queue>

/**
 * Mask a texture being rendered, floats from
**/

struct Coordinate{
    float x;
    float y;
    Coordinate(float a, float b){
        x = a;
        y = b;
    }
};

struct MaskPoint{
    int fieldNum;
    Coordinate* tl;
    Coordinate* tr;
    Coordinate* bl;
    Coordinate* br;
    MaskPoint(int field, float a1, float a2, float b1, float b2, float c1, float c2, float d1, float d2){
        fieldNum = field;
        tl = new Coordinate(a1, a2);
        tr = new Coordinate(b1, b2);
        bl = new Coordinate(c1, c2);
        br = new Coordinate(d1, d2);
    }
    MaskPoint(int field, Coordinate* a, Coordinate* b, Coordinate* c, Coordinate* d){
        fieldNum = field;
        tl = a;
        tr = b;
        bl = c;
        br = d;
    }
};

class MaskTexture: public TextureRender {
    public:
        MaskTexture(OverlayRenderer* render, TextureRender* parent);
        //add a point/keyframe to the fade
        void addPoint(MaskPoint *p);
    protected:
        //render this
        void Render(int field);
        //populate render array
        GLfloat* PopulateArray(int field);
    private:
        queue<MaskPoint> *points;
        MaskPoint *prevPoint;

        //opengl shader handles
        GLint positionLoc;
        GLint texCoordLoc;
        GLint samplerLoc;
};

#endif // MASKTEXTURE_H
