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

#ifndef JSONUTIL_H
#define JSONUTIL_H

#ifdef DUMPJSON
#ifndef JSONDIR
#define JSONDIR "json-dump/"
#endif
#endif

#include <jsoncpp/json/json.h>
#include <vector>
#include <string>
#include <time.h>

#include "logger.h"

class JsonUtil
{
    public:
        static Json::Value VectorToJSON(std::vector<std::string> vec);
        static std::vector<std::string> JSONToVector(Json::Value data);
        static bool parseJSON(std::string *msg, Json::Value *root);
        //check file exists
        static bool FileExists(const char * filename);
    protected:
    private:
};

#endif // JSONUTIL_H
