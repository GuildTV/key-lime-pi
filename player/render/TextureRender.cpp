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

#include "TextureRender.h"

#include "render/OverlayRenderer.h"

TextureRender::TextureRender(OverlayRenderer* render) {
    renderer = render;
    hasParent = false;
}

TextureRender::~TextureRender() {
    glDeleteTextures(1, &frame_texture);
    glDeleteFramebuffers(1, &frame_buffer);
    glDeleteProgram(programObject);
}

TextureRender::TextureRender(OverlayRenderer* render, TextureRender* rent) {
    renderer = render;
    parent = rent;
    hasParent = true;
}

GLuint TextureRender::Setup(const char *vShaderStr, const char *fShaderStr){

    if(renderer->CreateProgram(vShaderStr, fShaderStr, &programObject) == GL_FALSE)
        return GL_FALSE;

    // Use the program object
    glUseProgram(programObject);

    glGenFramebuffers(1, &frame_buffer);

    glGenTextures(1, &frame_texture);
    glBindTexture(GL_TEXTURE_2D, frame_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 576, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frame_texture, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
        return GL_TRUE;
    else
        return GL_FALSE;
}

void TextureRender::SwitchTo() {
    glUseProgram(programObject);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void TextureRender::RenderToTexture(int field){
    if(hasParent)
        parent->RenderToTexture(field);

    SwitchTo();
    Render(field);
}
