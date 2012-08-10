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

#include "LimeShared.h"

LimeShared::LimeShared() {
    run = false;
    videoLoaded = false;
    videoPlaying = false;
#ifdef RENDERTEST
    renderer = new OverlayRenderer(&up);
#endif
}

void LimeShared::VideoLoad(std::string name, std::string script){
    //complain if already playing
    if(videoPlaying){
        up.GetClient()->SendMessage("{\"type\":\"preloadVideo\",\"status\":\"already playing\"}");
        return;
    }

    //set as not loaded
    videoLoaded = false;
    FLog::Log(FLOG_INFO, "LimeShared::VideoLoad - Starting preload of \"%s\"", script.c_str());
    //generate paths to video and script
    std::string pathVid = DATAFOLDER;
    pathVid += script;
    pathVid += "/video.mp4";
    std::string pathJson = DATAFOLDER;
    pathJson += script;
    pathJson += "/script.json";

    //verify files exist
    if(!FileExists(pathVid.c_str())){
        FLog::Log(FLOG_ERROR, "LimeShared::VideoLoad - Couldnt find video file for \"%s\"", script.c_str());
        up.GetClient()->SendMessage("{\"type\":\"preloadVideo\",\"status\":\"video doesnt exist\"}");
        return;
    }
    if(!FileExists(pathJson.c_str())){
        FLog::Log(FLOG_ERROR, "LimeShared::VideoLoad - Couldnt find script file for \"%s\"", script.c_str());
        up.GetClient()->SendMessage("{\"type\":\"preloadVideo\",\"status\":\"script doesnt exist\"}");
        return;
    }

#ifndef RENDERTEST
    //load video
    wrap = new OMXWrapper(&up, &videoPlaying);
    wrap->Load(pathVid);//convert to bool or int?

#else
    //load gl stuff
    renderer->Create(pathJson);

#ifdef LIMEMASTER
    //draw prevideo frame
    renderer->PreDraw();
#endif

#endif

    std::string msg = "{\"type\":\"preloadVideo\",\"name\":\"";
    msg += name;
    msg += "\",\"status\":\"video loaded\"}";
    up.GetClient()->SendMessage(msg);

    //set as loaded
    videoLoaded = true;
}

void LimeShared::VideoPlay() {
    //complain if video isnt loaded
    if(!videoLoaded){
        up.GetClient()->SendMessage("{\"type\":\"playVideo\",\"status\":\"nothing loaded\"}");
        return;
    }

    //complain if already playing
    if(videoPlaying){
        up.GetClient()->SendMessage("{\"type\":\"playVideo\",\"status\":\"already playing\"}");
        return;
    }

    videoLoaded = false;
    videoPlaying = true;

    up.GetClient()->SendMessage("{\"type\":\"playVideo\",\"status\":\"starting playback\"}");

#ifndef RENDERTEST
    //play video
    wrap->Play();//is bool for success starting

#else

    //play gl stuff
    renderer->Run();
    videoPlaying = false;
#endif
}

void LimeShared::VideoStop(){
#ifndef RENDERTEST
    //stop video
    wrap->Stop();
#else
    //TODO stop code
    FLog::Log(FLOG_INFO, "LimeShared::VideoStop - Recieved");
#endif
}

bool LimeShared::CreateServer(const char * port){
    //create server for upstream
    int ret = up.CreateServer(port);
    if(ret != 0) {
        FLog::Log(FLOG_ERROR, "LimeShared::Run - failed to create server. Closing program");
        Stop();
        return false;
    }

    return true;
}

bool LimeShared::FinishSetup(){
    //create the sync timer
    limeTimer = new LimeTimer(this);

    //handle messages, whilst we are running
    while(run && up.ThreadRunning()){
        //get next message
        NetMessage* msg = up.GetMessageQueue()->Pop(true);
        HandleMessage(msg);
    }
    run = false;

    return true;
}

bool LimeShared::FileExists(const char * filename) {
    if (FILE * file = fopen(filename, "r")) {
        fclose(file);
        return true;
    }
    return false;
}

bool LimeShared::parseJSON(std::string *msg, Json::Value *root){
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

        string path = JSONDIR;
        path += formatted;

        string body = reader.getFormattedErrorMessages();
        body += "\n\n";
        body += *msg;

        FILE* f = fopen(path.c_str(), "w"); //TODO check file exists
        fwrite(body.c_str(), 1, body.length(), f);
        fclose(f);

        FLog::Log(FLOG_ERROR, "LimeShared::HandleMessage - Failed to parse json. Dumped to \"%s\"", path.c_str());
#else
        FLog::Log(FLOG_ERROR, "LimeShared::HandleMessage - Failed to parse json.");
#endif
        return false;
    }

    return true;
}

void LimeShared::HandleMessage(NetMessage* msg){
    FLog::Log(FLOG_DEBUG, "LimeShared::HandleMessage - Handling message");

    //parse json messages
    Json::Value root;
    if(!parseJSON(&msg->message, &root))
        return;

    //check type exists
    if(!root.isMember("type")){
        FLog::Log(FLOG_ERROR, "LimeShared::HandleMessage - Recieved message without a type");
        return;
    }

    if(HandleMessageEarly(msg, &root))
        return;

    const string type = root["type"].asString();

    //preload a video to be played
    if(type.compare("preloadVideo") == 0){
        HandleMessagePreload(msg, &root);

    } else if (type.compare("playVideo") == 0){
        HandleMessagePlay(msg, &root);

    }

    HandleMessageMore(msg, &root);
}

void LimeShared::HandleMessagePreload(NetMessage *msg, Json::Value *root){
    //check name exists
    if(!root->isMember("data")){
        FLog::Log(FLOG_ERROR, "LimeShared::HandleMessage - Recieved 'preloadVideo' command without a 'data' field");
        return;
    }
    Json::Value data = (*root)["data"];
    if(!data.isMember("script")){
        FLog::Log(FLOG_ERROR, "LimeShared::HandleMessage - Recieved 'preloadVideo' command without a 'data::script' field");
        return;
    }
    const string script = data["script"].asString();
    if(!data.isMember("name")){
        FLog::Log(FLOG_ERROR, "LimeShared::HandleMessage - Recieved 'preloadVideo' command without a 'data::name' field");
        return;
    }
    const string name = data["name"].asString();

    //forward message to slave
    preloadProcess(msg);

    //load video
    VideoLoad(name, script);//TODO pass data
}

void LimeShared::HandleMessagePlay(NetMessage *msg, Json::Value *root){
    if(!videoLoaded){
        up.GetClient()->SendMessage("{\"type\":\"playVideo\",\"status\":\"nothing loaded\"}");
        return;
    }

    long nano, sec;

    playProcess(root, &sec, &nano);

    //play video at specified time
    limeTimer->VideoPlay(sec, nano);
}
