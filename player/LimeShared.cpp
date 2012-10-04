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
}

void LimeShared::VideoPlay(std::string script){
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
    if(!JsonUtil::FileExists(pathVid.c_str())){
        FLog::Log(FLOG_ERROR, "LimeShared::VideoLoad - Couldnt find video file for \"%s\"", script.c_str());
        up.GetClient()->SendMessage("{\"type\":\"preloadVideo\",\"status\":\"video doesnt exist\"}");
        return;
    }
    if(!JsonUtil::FileExists(pathJson.c_str())){
        FLog::Log(FLOG_ERROR, "LimeShared::VideoLoad - Couldnt find script file for \"%s\"", script.c_str());
        up.GetClient()->SendMessage("{\"type\":\"preloadVideo\",\"status\":\"script doesnt exist\"}");
        return;
    }

    videoPlaying = true;

#ifndef RENDERTEST
    //load video
    wrap = new OMXWrapper(&up, &videoPlaying);
    wrap->Load(pathVid);//convert to bool or int?

#endif

    up.GetClient()->SendMessage("{\"type\":\"playVideo\",\"status\":\"starting playback\"}");

#ifndef RENDERTEST
    //play video
    wrap->Play();//is bool for success starting

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

vector<string> LimeShared::ListFiles(const char * path){
    vector<string> vec;
    DIR* dirp = opendir(path);
    dirent* dp;
    while ((dp = readdir(dirp)) != NULL){
        if(strcmp(dp->d_name, "..") != 0 && strcmp(dp->d_name, ".") != 0)
            vec.push_back(dp->d_name);
    }
    closedir(dirp);

    return vec;
}

void LimeShared::HandleMessage(NetMessage* msg){
    FLog::Log(FLOG_DEBUG, "LimeShared::HandleMessage - Handling message");

    //parse json messages
    Json::Value root;
    if(!JsonUtil::parseJSON(&msg->message, &root))
        return;

    //check type exists
    if(!root.isMember("type")){
        FLog::Log(FLOG_ERROR, "LimeShared::HandleMessage - Recieved message without a type");
        return;
    }

    if(HandleMessageEarly(msg, &root))
        return;

    const string type = root["type"].asString();

    if (type.compare("playVideo") == 0){
        HandleMessagePlay(msg, &root);

    } else if (type.compare("dataList") == 0){
        dataListProcess(&root);

    }

    HandleMessageMore(msg, &root);
}

void LimeShared::HandleMessagePlay(NetMessage *msg, Json::Value *root){

    if(!(*root).isMember("script")){
        FLog::Log(FLOG_ERROR, "LimeShared::HandleMessage - Recieved 'playVideo' command without a 'script' field");
        return;
    }
    const string script = (*root)["script"].asString();

    long nano, sec;

    playProcess(root, &sec, &nano);

    //play video at specified time
    limeTimer->VideoPlay(script, sec, nano);
}

