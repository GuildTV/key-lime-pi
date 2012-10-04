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

#include "LimeMaster.h"

#define VIDEOOFFSETSEC  2
#define VIDEOOFFSETNANO 0

LimeMaster::LimeMaster() {
    videoPlaying = false;
}

void LimeMaster::Run(char *addr) {
    run = true;

    if(!CreateServer(CONTROLPORT))
        return;

    //connection to slave
    if(pi.CreateClient(addr, PIPORT) != 0) {
        FLog::Log(FLOG_ERROR, "LimeMaster::Run - failed to connect to slave '%s'", addr);
        printf("Could not connect to pi at '%s'\n", addr);
        return;
    }

    //start listening to whatever pi is saying to us
    downstream = new LimeMasterDownStream(this, &pi);
    downstream->Start();

    //finish setup
    FinishSetup();
}

void LimeMaster::HandleMessageDown(NetMessage* msg){
    FLog::Log(FLOG_DEBUG, "LimeMaster::HandleMessageDown - Handling message from slave-pi");

    //parse json messages
    Json::Value root;
    if(!JsonUtil::parseJSON(&msg->message, &root))
        return;

    //check type exists
    if(!root.isMember("type")){
        FLog::Log(FLOG_ERROR, "LimeMaster::HandleMessageDown - Recieved message without a type");
        return;
    }

    const string type = root["type"].asString();

    if (type.compare("dataList") == 0){
        //check type exists
        if(!root.isMember("data")){
            FLog::Log(FLOG_ERROR, "LimeMaster::HandleMessageDown - Recieved dataList command without a data array");
            return;
        }

        vector<string> dat1 = ListFiles(DATAFOLDER);
        vector<string> dat2 = JsonUtil::JSONToVector(root["data"]);

        vector<string> overlap;

        for(int o = 0; o < dat1.size(); o++){
            for(int i = 0; i < dat2.size(); i++){
                if(dat1[o] == dat2[i]){
                    overlap.push_back(dat2[i]);
                    goto cont;
                }
            }

            cont:
                continue;
        }

        Json::Value list = JsonUtil::VectorToJSON(overlap);
        Json::FastWriter writer;

        std::string json = "{\"type\":\"dataList\",\"data\":";
        json += writer.write(list);
        json.resize(json.size()-1);
        json += "}";
        up.GetClient()->SendMessage(json);
        printf(json.c_str());
    }
}

bool LimeMaster::HandleMessageEarly(NetMessage *msg, Json::Value* root){
    return false;
}

void LimeMaster::playProcess(Json::Value *root, long *sec, long *nano){
    //get time
    timespec time;
    clock_gettime(CLOCK_REALTIME, &time);

    //calculate start time for video
    *nano = time.tv_nsec + VIDEOOFFSETNANO;
    *sec = time.tv_sec + VIDEOOFFSETSEC;
    if(*nano >= 1000000000){
        *nano -=1000000000;
        *sec += 1;
    }

    //generate json string for slave
    string format = "{\"type\":\"playVideo\",\"second\":%d,\"nanosecond\":%d}";
    char formatted[format.length()+10];
    sprintf(formatted, format.c_str(), *sec, *nano);

    //send message to slave
    pi.GetClient()->SendMessage(formatted);
}

void LimeMaster::dataListProcess(Json::Value *root){
    pi.GetClient()->SendMessage("{\"type\":\"dataList\"}");
}

int main(int argc, char *argv[]){

    if(argc != 2) {
        printf("USAGE: master.bin <slave ip>\n\n");
        return 0;
    }

    //open log
    FLog::Open("Master.log");

    FLog::Log(FLOG_INFO, "Starting key-lime-pi master");
    printf("Starting key-lime-pi master\n");

    //create and run program
    LimeMaster lime;
    lime.Run(argv[1]);

    printf("Closing key-lime-pi master\n");
    FLog::Log(FLOG_INFO, "Closing key-lime-pi master\n\n");

    //close log
    FLog::Close();
    return 0;
}
