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

#ifndef PNGTEXTURE_H
#define PNGTEXTURE_H

#define PNG_SKIP_SETJMP_CHECK
#include <png.h>
#define TEXTURE_LOAD_ERROR 0

#include "render/TextureRender.h"

class PNGTexture: public TextureRender {
    public:
        PNGTexture(OverlayRenderer* render);

        void LoadPNG(string filename);
    protected:
        //render this
        void Render(int field);

        GLuint loadTexture(const string filename, int &width, int &height);
    private:
        //opengl shader handles
        GLint positionLoc;
        GLint texCoordLoc;
        GLint samplerLoc;

        GLuint pngTex;
};

#endif // PNGTEXTURE_H
