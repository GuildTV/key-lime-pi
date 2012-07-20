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

#include "RenderTest.h"
#include <jsoncpp/json/json.h>
#include <time.h>
#include <stdio.h>

#ifdef DUMPJSON
#ifndef JSONDIR
#define JSONDIR "json-dump/"
#endif
#endif

#ifndef DATAFOLDER
#define DATAFOLDER "data/"
#endif

RenderTest::RenderTest() {
    run = false;
    videoLoaded = false;
}

RenderTest::~RenderTest() {
    //dtor
}

void RenderTest::Run() {
    run = true;

    int ret = net.CreateServer("6789");
    if(ret != 0) {

        FLog::Log(FLOG_ERROR, "RenderTest::Run - failed to create server. Closing program");
        Stop();
        return;
    }

    while(run && net.ThreadRunning()){
        //get next message
        NetMessage* msg = (*net.GetMessageQueue()).Pop(true);
        HandleMessage(msg);
    }
    run = false;
}

void RenderTest::HandleMessage(NetMessage* msg){
    FLog::Log(FLOG_DEBUG, "RenderTest::HandleMessage - Handling message");
    Json::Value root;
    Json::Reader reader;
    bool parsedSuccess = reader.parse((*msg).message, root, false);
    if(!parsedSuccess){
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

        FLog::Log(FLOG_ERROR, "RenderTest::HandleMessage - Failed to parse json. Dumped to \"%s\"", path.c_str());
#else
        FLog::Log(FLOG_ERROR, "RenderTest::HandleMessage - Failed to parse json.");
#endif
        return;
    }

    const string type = root["type"].asString();

    //preload a video to be played
    if(type.compare("preloadVideo") == 0){
        const string name = root["name"].asString();

        VideoLoad(name);

    } else if (type.compare("playVideo") == 0){

        VideoPlay();

    }

}

void RenderTest::VideoLoad(std::string name){
    videoLoaded = false;
    FLog::Log(FLOG_INFO, "RenderTest::VideoLoad - Starting preload of \"%s\"", name.c_str());
    std::string pathVid = DATAFOLDER;
    pathVid += name;
    pathVid += "/video.mp4";
    std::string pathJson = DATAFOLDER;
    pathJson += name;
    pathJson += "/script.json";

    //verify files exist
    if(!FileExists(pathVid.c_str())){
        FLog::Log(FLOG_ERROR, "RenderTest::VideoLoad - Couldnt find video file for \"%s\"", name.c_str());
        (*net.GetClient()).SendMessage("{\"type\":\"preloadVideo\",\"status\":\"failed\"}"); //TODO better message to whoever
        return;
    }
    if(!FileExists(pathJson.c_str())){
        FLog::Log(FLOG_ERROR, "RenderTest::VideoLoad - Couldnt find script file for \"%s\"", name.c_str());
        (*net.GetClient()).SendMessage("{\"type\":\"preloadVideo\",\"status\":\"failed\"}"); //TODO better message to whoever
        return;
    }

#ifndef RENDERTEST
    //load video
    wrap = new OMXWrapper;
    wrap->Load(pathVid);//convert to bool or int?

#else
    //load gl stuff

    renderer = new OverlayRenderer;
    renderer->Create(pathJson);

    renderer->PreDraw();
#endif

    videoLoaded = true;
}

void RenderTest::VideoPlay() {
    if(!videoLoaded){
        (*net.GetClient()).SendMessage("{\"type\":\"playVideo\",\"status\":\"nothing loaded\"}"); //TODO better message to whoever
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

bool RenderTest::FileExists(const char * filename) {
    if (FILE * file = fopen(filename, "r")) {
        fclose(file);
        return true;
    }
    return false;
}


int main(int argc, char *argv[]){
    FLog::Open("Renderer.log");

    FLog::Log(FLOG_INFO, "Starting render test");
    printf("Starting render test");

    RenderTest render;
    render.Run();

    printf("Closing render test");
    FLog::Log(FLOG_INFO, "Closing render test\n\n");

    FLog::Close();
    return 0;
}
