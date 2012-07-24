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

#ifndef FREETYPE_H
#define FREETYPE_H

#include <GLES2/gl2.h>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H


#include "logger.h"

#define MAXCHARVALUE 127
#define DEFAULTCHARVALUE 45

using namespace std;

struct TextChar {
    TextChar() :loaded(false), texture(-1){}
    GLuint texture;
    int width;
    int height;
    int advanceX;
    int advanceY;
    int bitTop;
    int bitLeft;
    bool loaded;
};

//forward declare overlayrenderer
class OverlayRenderer;

class Freetype
{
    public:
        Freetype(OverlayRenderer *render);
        virtual ~Freetype();
        bool LoadFreetypeRange(string font_file, int height, int start, int end, TextChar *storage);
        void WriteString(char * text, TextChar *charSet, int x, int y, float scaleX, float scaleY);
        unsigned int pow2(unsigned int num);

    protected:
        bool LoadFreetype(string font_file);
        bool LoadFreetypeChar(int height, int value, TextChar *character);
        void CloseFreetype();
        void FreetypeToTexture(FT_GlyphSlot *slot, TextChar* character);

    private:
        FT_Library library;
        FT_Face face;

        OverlayRenderer *renderer;
};

#endif // FREETYPE_H
