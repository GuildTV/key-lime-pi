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

#ifndef LIMESHARED_H
#define LIMESHARED_H

#ifdef DUMPJSON
#ifndef JSONDIR
#define JSONDIR "json-dump/"
#endif
#endif

#ifndef DATAFOLDER
#define DATAFOLDER "data/"
#endif

#ifndef PIPORT
#define PIPORT "6788"
#endif
#ifndef CONTROLPORT
#define CONTROLPORT "6789"
#endif

#include "net/NetIO.h"
#include <string>
#include "logger.h"
#include "LimeTimer.h"
#include <jsoncpp/json/json.h>
#include <time.h>
#include <stdio.h>

#ifdef RENDERTEST
#include "render/OverlayRenderer.h"
#else
#include "OMXWrapper.h"
#endif

class LimeShared
{
    public:
        LimeShared();
        //load video
        void VideoLoad(std::string name, std::string script);
        //start videoplayback
        void VideoPlay();
        //stop video playback
        void VideoStop();
        //stop program
        void Stop(){run=false;};
        //check file exists
        bool FileExists(const char * filename);
        //get netio
        NetIO getNetUp(){return up;};
        //create server
        bool CreateServer(const char * port);
        //finish setting up program
        bool FinishSetup();

    protected:
        //convert msg to json
        bool parseJSON(std::string *msg, Json::Value *root);

        //network connection upstream
        NetIO up;
        //is program running?
        bool run;
        //is a video loaded
        bool videoLoaded;
        //is a video playing
        bool videoPlaying;
        //timer for video start cync
        LimeTimer* limeTimer;
        //handle a recieved message
        void HandleMessage(NetMessage* msg);

        virtual void HandleMessageMore(NetMessage *msg, Json::Value* root){};
        virtual bool HandleMessageEarly(NetMessage *msg, Json::Value* root){return false;};

        virtual void preloadProcess(NetMessage *msg){};
        virtual void playProcess(Json::Value *root, long *sec, long *nano) = 0;

        void HandleMessagePreload(NetMessage *msg, Json::Value* root);
        void HandleMessagePlay(NetMessage *msg, Json::Value* root);

        //pointer to video player/overlayrenderer
#ifdef RENDERTEST
        OverlayRenderer* renderer;
#else
        OMXWrapper* wrap;
#endif

    private:

};

#endif // LIMESHARED_H
