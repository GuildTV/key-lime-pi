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

#include "LimeSlave.h"
#include <jsoncpp/json/json.h>
#include <time.h>
#include <stdio.h>

#ifdef DUMPJSON
#ifndef JSONDIR
#define JSONDIR "json-dump/"
#endif
#endif

#ifndef DATAFOLDER
#define DATAFOLDER "data-slave/"
#endif

#ifndef PIPORT
#define PIPORT "6788"
#endif
#ifndef CONTROLPORT
#define CONTROLPORT "6789"
#endif

LimeSlave::LimeSlave() {
    run = false;
    videoLoaded = false;
#ifdef RENDERTEST
    renderer = new OverlayRenderer;
#endif
}

LimeSlave::~LimeSlave() {
    //dtor
}

void LimeSlave::Run() {
    run = true;

    //create server
    int ret = pi.CreateServer(PIPORT);
    if(ret != 0) {
        FLog::Log(FLOG_ERROR, "LimeSlave::Run - failed to create server. Closing program");
        Stop();
        return;
    }

    //finish setup
    FinishSetup();
}

bool LimeSlave::FinishSetup(){
    //create the sync timer
    limeTimer = new LimeTimer(this);

    //handle messages, whilst we are running
    while(run && pi.ThreadRunning()){
        //get next message
        NetMessage* msg = pi.GetMessageQueue()->Pop(true);
        HandleMessage(msg);
    }
    run = false;

    return true;
}

void LimeSlave::HandleMessage(NetMessage* msg){
    FLog::Log(FLOG_DEBUG, "LimeSlave::HandleMessage - Handling message");

    //parse json messages
    Json::Value root;
    Json::Reader reader;
    bool parsedSuccess = reader.parse(msg->message, root, false);
    if(!parsedSuccess){
//dump json, if enabled at compile time
#ifdef DUMPJSON
        time_t rawtime;
        struct tm * time;

        std::time(&rawtime);
        time = std::localtime (&rawtime);

        char formatted[25];
        sprintf(formatted, "dump-%d-%d_%d.%d.%d.dump", time->tm_mday, time->tm_mon, time->tm_hour, time->tm_min, time->tm_sec);

        string path = JSONDIR;
        path += formatted;

        string body = reader.getFormattedErrorMessages();
        body += "\n\n";
        body += (*msg).message;

        FILE* f = fopen(path.c_str(), "w"); //TODO check file exists
        fwrite(body.c_str(), 1, body.length(), f);
        fclose(f);

        FLog::Log(FLOG_ERROR, "LimeSlave::HandleMessage - Failed to parse json. Dumped to \"%s\"", path.c_str());
#else
        FLog::Log(FLOG_ERROR, "LimeSlave::HandleMessage - Failed to parse json.");
#endif
        return;
    }

    //check type exists
    if(!root.isMember("type")){
        FLog::Log(FLOG_ERROR, "LimeMaster::HandleMessage - Recieved message without a type");
        return;
    }
    const string type = root["type"].asString();

    //preload a video to be played
    if(type.compare("preloadVideo") == 0){
        //check name exists
        if(!root.isMember("name")){
            FLog::Log(FLOG_ERROR, "LimeMaster::HandleMessage - Recieved 'preloadVideo' command without a 'name' field");
            return;
        }
        const string name = root["name"].asString();

        //load video
        VideoLoad(name);

    } else if (type.compare("playVideo") == 0){
        //check second and nanosecond exist
        if(!root.isMember("second") || !root.isMember("nanosecond")){
            FLog::Log(FLOG_ERROR, "LimeMaster::HandleMessage - Recieved 'playVideo' command without complete timings");
            return;
        }

        //get times
        long sec = root["second"].asInt64();
        long nano = root["nanosecond"].asInt64();

        //play video at specified time
        limeTimer->VideoPlay(sec, nano);
    }

}

void LimeSlave::VideoLoad(std::string name){
    //set as not loaded
    videoLoaded = false;
    FLog::Log(FLOG_INFO, "LimeSlave::VideoLoad - Starting preload of \"%s\"", name.c_str());
    //generate paths to video and script
    std::string pathVid = DATAFOLDER;
    pathVid += name;
    pathVid += "/video.mp4";
    std::string pathJson = DATAFOLDER;
    pathJson += name;
    pathJson += "/script.json";

    //verify files exist
    if(!FileExists(pathVid.c_str())){
        FLog::Log(FLOG_ERROR, "LimeSlave::VideoLoad - Couldnt find video file for \"%s\"", name.c_str());
        pi.GetClient()->SendMessage("{\"type\":\"preloadVideo\",\"status\":\"failed\"}"); //TODO better message to whoever
        return;
    }
    if(!FileExists(pathJson.c_str())){
        FLog::Log(FLOG_ERROR, "LimeSlave::VideoLoad - Couldnt find script file for \"%s\"", name.c_str());
        pi.GetClient()->SendMessage("{\"type\":\"preloadVideo\",\"status\":\"failed\"}"); //TODO better message to whoever
        return;
    }

#ifndef RENDERTEST
    //load video
    wrap = new OMXWrapper;
    wrap->Load(pathVid);//convert to bool or int?

#else
    //load gl stuff
    renderer->Create(pathJson);

#endif

    //set as loaded
    videoLoaded = true;
}

void LimeSlave::VideoPlay() {
    //complain if video isnt loaded
    if(!videoLoaded){
        pi.GetClient()->SendMessage("{\"type\":\"playVideo\",\"status\":\"nothing loaded\"}"); //TODO better message to whoever
        return;
    }
    videoLoaded = false;

#ifndef RENDERTEST
    //play video
    wrap->Play();//is bool for success starting

#else

    //play gl stuff
    renderer->Run();
#endif
}

void LimeSlave::VideoStop(){
#ifndef RENDERTEST
    //stop video
    wrap->Stop();
#else
    //TODO stop code
    FLog::Log(FLOG_INFO, "LimeSlave::VideoStop - Recieved");
#endif
}

bool LimeSlave::FileExists(const char * filename) {
    if (FILE * file = fopen(filename, "r")) {
        fclose(file);
        return true;
    }
    return false;
}


int main(int argc, char *argv[]){
    //open log
    FLog::Open("Slave.log");

    FLog::Log(FLOG_INFO, "Starting render test");
    printf("Starting render test\n");

    //create and run program
    LimeSlave lime;
    lime.Run();

    printf("Closing render test\n");
    FLog::Log(FLOG_INFO, "Closing render test\n\n");

    //close log
    FLog::Close();
    return 0;
}
