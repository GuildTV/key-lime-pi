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

#define DATAFOLDER "data-slave/"

#include "LimeSlave.h"

void LimeSlave::Run() {
    run = true;

    if(!CreateServer(PIPORT))
        return;

    //finish setup
    FinishSetup();
}

void LimeSlave::playProcess(Json::Value *root, long *sec, long *nano){
    *sec = (*root)["second"].asInt64();
    *nano = (*root)["nanosecond"].asInt64();
}

void LimeSlave::dataListProcess(Json::Value *root){
    vector<string> vec = ListFiles(DATAFOLDER);
    Json::Value list = JsonUtil::VectorToJSON(vec);

    Json::FastWriter writer;

    std::string json = "{\"type\":\"dataList\",\"data\":";
    json += writer.write(list);
    json.resize(json.size()-1);
    json += "}";
    up.GetClient()->SendMessage(json);
}

int main(int argc, char *argv[]){
    //open log
    FLog::Open("Slave.log");

    FLog::Log(FLOG_INFO, "Starting key-lime-pi slave");
    printf("Starting key-lime-pi slave\n");

    //create and run program
    LimeSlave lime;
    lime.Run();

    printf("Closing key-lime-pi slave\n");
    FLog::Log(FLOG_INFO, "Closing key-lime-pi slave\n\n");

    //close log
    FLog::Close();
    return 0;
}
