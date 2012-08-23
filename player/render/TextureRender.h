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

#ifndef TEXTURERENDER_H
#define TEXTURERENDER_H

#include <GLES2/gl2.h>
#include "render/Freetype.h"

class OverlayRenderer;

class TextureRender
{
    public:
        TextureRender(OverlayRenderer* render);
        TextureRender(OverlayRenderer* render, TextureRender* rent);
        ~TextureRender();
        //render this to its texture
        void RenderToTexture();
        //setup this with specified shaders
        GLuint Setup(const char *vShaderStr, const char *fShaderStr);
        //get the texture handle
        GLuint getTexture(){return frame_texture;};
        //get the parent
        TextureRender* getParent(){return parent;};

    protected:
        //render the next frame of this
        virtual void Render() = 0;
        //get the overlay renderer
        OverlayRenderer* getRenderer() {return renderer;};

        //switch to this program and framebuffer
        void SwitchTo();

        //opengl program object
        GLuint programObject;

    private:
        //renderer handle
        OverlayRenderer* renderer;
        //opengl framebuffer handles
        GLuint frame_buffer;
        GLuint frame_texture;

        //store parent render
        TextureRender* parent;
        bool hasParent;
};

#endif // TEXTURERENDER_H
