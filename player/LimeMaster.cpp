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

void LimeMaster::Run() {
    run = true;

    if(!CreateServer(CONTROLPORT))
        return;

    //connection to slave
    string addr = "192.168.1.99";
    if(pi.CreateClient(addr, PIPORT) != 0) {
        FLog::Log(FLOG_ERROR, "LimeMaster::Run - failed to connect to slave '%s'", addr.c_str());
        //try to connect with addresses sent from control
        while(!piConnected){
            NetMessage* msg = up.GetMessageQueue()->Pop(true);
            HandleMessage(msg);
        }
    }

    //set as slave connected
    piConnected = true;

    //finish setup
    FinishSetup();
}

void LimeMaster::HandleMessageMore(NetMessage *msg, Json::Value* root){
    const string type = (*root)["type"].asString();
    if (type.compare("previewVideo") == 0){
        //check name exists
        if(!root->isMember("data")){
            FLog::Log(FLOG_ERROR, "LimeMaster::HandleMessage - Recieved 'previewVideo' command without a 'data' field");
            return;
        }
        Json::Value data = (*root)["data"];
        if(!data.isMember("script")){
            FLog::Log(FLOG_ERROR, "LimeMaster::HandleMessage - Recieved 'previewVideo' command without a 'data::script' field");
            return;
        }
        const string script = data["script"].asString();
        if(!data.isMember("name")){
            FLog::Log(FLOG_ERROR, "LimeMaster::HandleMessage - Recieved 'previewVideo' command without a 'data::name' field");
            return;
        }
        const string name = data["name"].asString();

        //play video at specified time
        VideoPreview(name, script);
    }
}

bool LimeMaster::HandleMessageEarly(NetMessage *msg, Json::Value* root){
    const string type = (*root)["type"].asString();

    if(type.compare("slaveConnected") == 0){
        if(piConnected)
            up.GetClient()->SendMessage("{\"type\":\"slaveConnected\",\"status\":\"true\"}");
        else
            up.GetClient()->SendMessage("{\"type\":\"slaveConnected\",\"status\":\"false\"}");
        return true;
    }

    //if slave is not connected
    if(!piConnected) {
        if(type.compare("slaveAddress") == 0){
            //check address exists
            if(!root->isMember("address")){
                FLog::Log(FLOG_ERROR, "LimeMaster::HandleMessage - Recieved 'slaveAddress' command without a 'address' field");
                return true;//TODO inform control
            }
            string addr = (*root)["address"].asString();

            //try to connect to new address
            if(pi.CreateClient(addr, PIPORT) == 0){
                piConnected = true;
            } else {
                FLog::Log(FLOG_ERROR, "LimeMaster::HandleMessage - failed to connect to slave '%s'", addr.c_str());
            }
        } else {
            //unexpected message recieved
            FLog::Log(FLOG_ERROR, "LimeMaster::HandleMessage - Recieved unexpected message, when not connected to slave");
            //TODO inform control
        }
        return true;
    }

    return false;
}

void LimeMaster::VideoPreview(std::string name, std::string script) {
    //complain if already playing
    if(videoPlaying){
        up.GetClient()->SendMessage("{\"type\":\"previewVideo\",\"status\":\"already playing\"}");
        return;
    }

    //set as not loaded
    videoLoaded = false;
    FLog::Log(FLOG_INFO, "LimeMaster::VideoLoad - Starting preview of \"%s\"", script.c_str());
    //generate paths to video and script
    std::string pathVid = DATAFOLDER;
    pathVid += script;
    pathVid += "/video.mp4";
    std::string pathJson = DATAFOLDER;
    pathJson += script;
    pathJson += "/script.json";

    //verify files exist
    if(!FileExists(pathVid.c_str())){
        FLog::Log(FLOG_ERROR, "LimeMaster::VideoLoad - Couldnt find video file for \"%s\"", script.c_str());
        up.GetClient()->SendMessage("{\"type\":\"previewVideo\",\"status\":\"video doesnt exist\"}");
        return;
    }
    if(!FileExists(pathJson.c_str())){
        FLog::Log(FLOG_ERROR, "LimeMaster::VideoLoad - Couldnt find script file for \"%s\"", script.c_str());
        up.GetClient()->SendMessage("{\"type\":\"previewVideo\",\"status\":\"script doesnt exist\"}");
        return;
    }

#ifndef RENDERTEST
    //load video
    wrap = new OMXWrapper(&up, &videoPlaying);
    wrap->Load(pathVid);//convert to bool or int?

#else
    //load gl stuff
    renderer->Create(pathJson);

    //draw prevideo frame
    renderer->PreDraw();

#endif
    //set as loaded
    videoLoaded = true;
    videoPlaying = true;

    up.GetClient()->SendMessage("{\"type\":\"previewVideo\",\"status\":\"starting playback\"}");

#ifndef RENDERTEST
    //play video
    wrap->Play();//is bool for success starting

#else

    //play gl stuff
    renderer->Run();
    videoPlaying = false;
#endif
}

void LimeMaster::preloadProcess(NetMessage *msg){
    pi.GetClient()->SendMessage(msg->message);
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
    sprintf(formatted, format.c_str(), sec, nano);

    //send message to slave
    pi.GetClient()->SendMessage(formatted);
}

int main(int argc, char *argv[]){
    //open log
    FLog::Open("Master.log");

    FLog::Log(FLOG_INFO, "Starting key-lime-pi master");
    printf("Starting key-lime-pi master\n");

    //create and run program
    LimeMaster lime;
    lime.Run();

    printf("Closing key-lime-pi master\n");
    FLog::Log(FLOG_INFO, "Closing key-lime-pi master\n\n");

    //close log
    FLog::Close();
    return 0;
}
