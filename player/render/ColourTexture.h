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

#ifndef COLOURTEXTURE_H
#define COLOURTEXTURE_H

#include "render/TextureRender.h"

class ColourTexture: public TextureRender {
    public:
        ColourTexture(OverlayRenderer* render, TextureRender* parent);
        void SetColour(float r, float g, float b, float a);
    protected:
        //render this
        void Render();
    private:
        GLfloat colour[4];

        //opengl shader handles
        GLint positionLoc;
        GLint texCoordLoc;
        GLint samplerLoc;

        GLint colourLoc;
};

#endif // COLOURTEXTURE_H
