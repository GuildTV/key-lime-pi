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

#include "Freetype.h"

#include "render/OverlayRenderer.h"

Freetype::Freetype(OverlayRenderer *render) {
    renderer = render;
}

Freetype::~Freetype() {
    CloseFreetype();
}

bool Freetype::LoadFreetype(string font_file) {
    if(FT_Init_FreeType(&library)){
        FLog::Log(FLOG_ERROR, "Freetype::LoadFreetype - failed to load freetype library");
        return false;
    }

    if(FT_New_Face(library, font_file.c_str(), 0, &face)){
        FLog::Log(FLOG_ERROR, "Freetype::LoadFreetype - failed to load freetype font face");
        return false;
    }

    FLog::Log(FLOG_DEBUG, "Freetype::LoadFreetype - loaded freetype font file '%s'", font_file.c_str());
    return true;
}

bool Freetype::LoadFreetypeRange(string font_file, int height, int start, int end, TextChar *storage){
    if(!LoadFreetype(font_file)){
        return false;
    }

    if(start < 0 || end > MAXCHARVALUE){
        FLog::Log(FLOG_ERROR, "Freetype::LoadFreetypeRange - start or end index out of range");
        return false;
    }

    for(int i=start;i<=end;i++) {
        if(!LoadFreetypeChar(height, i, &storage[i])){
            return false;
        }
    }

    FLog::Log(FLOG_DEBUG, "Freetype::LoadFreetype - loaded freetype character range %d-%d", start, end);
    CloseFreetype();
    return true;
}

bool Freetype::LoadFreetypeChar(int height, int value, TextChar *character) {
    unsigned int h = height;
    FT_Set_Char_Size(face, h << 6, h << 6, 96, 96);

    unsigned char ch = value;

    if(FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_RENDER)){
        FLog::Log(FLOG_ERROR, "Freetype::LoadFreetype - failed to load freetype glyph");
        return false;
    }

    FT_GlyphSlot slot = face->glyph;
    FreetypeToTexture(&slot, character);

    return true;
}

void Freetype::FreetypeToTexture(FT_GlyphSlot *slot, TextChar* character){
    FT_Bitmap *bitmap = &(*slot)->bitmap;
    int width = pow2(bitmap->width);
    int height = pow2(bitmap->rows);
    GLubyte* expanded_data = new GLubyte[ 2 * width * height];

    character->width = bitmap->width;
    character->height = bitmap->rows;
    character->advanceX = (*slot)->advance.x;
    character->advanceY = (*slot)->advance.y;
    character->bitLeft = (*slot)->bitmap_left;
    character->bitTop = (*slot)->bitmap_top;
    character->loaded = true;

    for(int j = 0; j <height ; j++) {
        for(int i = 0; i < width; i++) {
            expanded_data[2 * (i + j * width)] = 255;
            expanded_data[2 * (i + j * width) + 1] = (i >= bitmap->width || j >= bitmap->rows) ? 0 : bitmap->buffer[i + bitmap->width * j];
        }
    }

    glGenTextures(1, &(*character).texture);
    glBindTexture(GL_TEXTURE_2D, (*character).texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE_ALPHA,width,height,0,GL_LUMINANCE_ALPHA,GL_UNSIGNED_BYTE,expanded_data);

    free((void *)expanded_data);
}

void Freetype::CloseFreetype() {
    FT_Done_Face(face);
    FT_Done_FreeType(library);
}

void Freetype::WriteString(char * text, TextChar *charSet, int x, int y, float scaleX, float scaleY) { //TODO - multiline text output?
#ifndef RENDERTEST
    scaleX *= 9.0f/16.0f; //skew for raspberry screen output, even though has been told aspect ratio is 16/9
#endif
    float xPos = x;
        xPos *= 2;
        xPos /= renderer->getWidth();
        xPos -= 1.0f;
    float yPos = y;
        yPos *= 2;
        yPos /= renderer->getHeight();
        yPos -= 1.0f;
    for (uint i=0;i<strlen(text);i++){
    unsigned char c = text[i];
    TextChar *ch = &charSet[c];
    if(!ch->loaded){
        *ch = charSet[DEFAULTCHARVALUE];
        if(!ch->loaded){
            FLog::Log(FLOG_ERROR, "Freetype::WriteString - default character wasnt loaded");
            return;
        }
    }

    float chH = pow2(ch->height);
        chH *= scaleY;
        chH *= 2.0f;
        chH /= renderer->getHeight();
    float chW = pow2(ch->width);
        chW *= scaleX;
        chW *= 2.0f;
        chW /= renderer->getWidth();

    float bL = ch->bitLeft;
        bL /= renderer->getWidth();
        bL *= scaleX;
        bL *= 2.0f;

    float bT = ch->bitTop;
        bT /= renderer->getHeight();
        bT *= scaleY;
        bT *= 2.0f;


    GLfloat vVertices[] = { xPos+bL,  yPos+bT, 0.0f,  // Position 0
                            0.0f,  0.0f,        // TexCoord 0
                            xPos+bL,  yPos-chH+bT, 0.0f,  // Position 1
                            0.0f,  1.0f,        // TexCoord 1
                            xPos+chW+bL,  yPos-chH+bT, 0.0f,  // Position 2
                            1.0f,  1.0f,        // TexCoord 2
                            xPos+chW+bL,  yPos+bT, 0.0f,  // Position 3
                            1.0f,  0.0f         // TexCoord 3
                         };
    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

    glVertexAttribPointer (renderer->getPosLoc(), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), vVertices );
    // Load the texture coordinate
    glVertexAttribPointer (renderer->getTexLoc(), 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), &vVertices[3] );

    glEnableVertexAttribArray ( renderer->getPosLoc() );
    glEnableVertexAttribArray ( renderer->getTexLoc() );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ch->texture);
    glUniform1i ( renderer->getSamLoc(), 0 );
    glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );

    float xInc = ch->advanceX >> 6;
        xInc *= scaleX;
        xInc *= 2.0f;
        xInc /= renderer->getWidth();
    xPos += xInc;
    }
}

unsigned int Freetype::pow2(unsigned int a) {
    unsigned int rval=2;
    // rval<<=1 Is A Prettier Way Of Writing rval*=2;
    while(rval<a) rval<<=1;
    return rval;
}
