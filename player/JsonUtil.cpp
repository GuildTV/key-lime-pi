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

#include "JsonUtil.h"

Json::Value JsonUtil::VectorToJSON(std::vector<std::string> vec){
    Json::Value arr(Json::arrayValue);
    for(auto it = vec.begin(); it < vec.end(); it++){
        arr.append(Json::Value(*it));
    }
    return arr;
}

std::vector<std::string> JsonUtil::JSONToVector(Json::Value data){
    std::vector<std::string> vec;

    if(!data.isArray())
        return vec;

    for(int i=0;i<data.size();i++){
        vec.push_back(data[i].asString());
    }

    return vec;
}

bool JsonUtil::parseJSON(std::string *msg, Json::Value *root){
    //parse json messages
    Json::Reader reader;
    bool parsedSuccess = reader.parse(*msg, *root, false);
    if(!parsedSuccess){
//dump json, if enabled at compile time
#ifdef DUMPJSON
        time_t rawtime;
        struct tm * time;

        std::time(&rawtime);
        time = std::localtime (&rawtime);

        char formatted[25];
        sprintf(formatted, "dump-%d-%d_%d.%d.%d.dump", time->tm_mday, time->tm_mon, time->tm_hour, time->tm_min, time->tm_sec);

        std::string path = JSONDIR;
        path += formatted;

        std::string body = reader.getFormattedErrorMessages();
        body += "\n\n";
        body += *msg;

        FILE* f = fopen(path.c_str(), "w"); //TODO check file exists
        fwrite(body.c_str(), 1, body.length(), f);
        fclose(f);

        FLog::Log(FLOG_ERROR, "JsonUtil::parseJSON - Failed to parse json. Dumped to \"%s\"", path.c_str());
#else
        FLog::Log(FLOG_ERROR, "JsonUtil::parseJSON - Failed to parse json.");
#endif
        return false;
    }

    return true;
}


bool JsonUtil::FileExists(const char * filename) {
    if (FILE * file = fopen(filename, "r")) {
        fclose(file);
        return true;
    }
    return false;
}
