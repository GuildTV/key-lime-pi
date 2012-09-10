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

#ifndef EFFECTPARSER_H
#define EFFECTPARSER_H

#include <jsoncpp/json/json.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>
#include <string>
#include <string.h>
#include <vector>

#include "JsonUtil.h"

#include "render/TextTexture.h"
#include "render/TextureRender.h"
#include "render/ColourTexture.h"
#include "render/FadeTexture.h"
#include "render/PNGTexture.h"
#include "render/SolidTexture.h"
#include "render/MaskTexture.h"

class OverlayRenderer;

class EffectParser {
    public:
        static bool Parse(OverlayRenderer* renderer, vector<TextureRender*> *renderElms, std::string file, Json::Value data);
    protected:
        static TextureRender* ParseEffect(OverlayRenderer* renderer, Json::Value effect, Json::Value *data);
    private:
};

#endif // EFFECTPARSER_H
