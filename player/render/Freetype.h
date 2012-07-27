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

/**
 * Render freetype characters to the screen via OpenGL ES2.0
**/

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

//a character of text that has been loaded
struct TextChar {
    int width;
    int height;
    int advanceX;
    int advanceY;
    int bitTop;
    int bitLeft;
    bool loaded;
    GLuint texture;
    TextChar() :loaded(false), texture(-1){}
};

//forward declare overlayrenderer
class OverlayRenderer;

class Freetype
{
    public:
        //setup freetype renderer
        Freetype(OverlayRenderer *render);
        virtual ~Freetype();
        //load a range of characters into an array
        bool LoadFreetypeRange(string font_file, int height, int start, int end, TextChar *storage);
        //write a string to screen, using character array
        void WriteString(char * text, TextChar *charSet, int x, int y, float scaleX, float scaleY);
        //calculate the next power of 2 above num
        unsigned int pow2(unsigned int num);

    protected:
        //load a freetype font
        bool LoadFreetype(string font_file);
        //load a freetype character
        bool LoadFreetypeChar(int height, int value, TextChar *character);
        //close freetype font
        void CloseFreetype();
        //convert a freetype character to opengl texture
        void FreetypeToTexture(FT_GlyphSlot *slot, TextChar* character);

    private:
        FT_Library library;
        FT_Face face;

        OverlayRenderer *renderer;
};

#endif // FREETYPE_H
