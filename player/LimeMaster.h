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

/**
 * Main program for key-lime-pi master device
**/

#ifndef LIMEMASTER_H
#define LIMEMASTER_H

#include "LimeShared.h"
#include "LimeMasterDownStream.h"

class LimeMaster: public LimeShared
{
    public:
        LimeMaster();
        //run program
        void Run();
        //handle a recieved message
        void HandleMessageDown(NetMessage* msg);

    protected:
        //load video
        void VideoPreview(std::string name, std::string script);

        //process preload command beyond scope of LimeShared
        void preloadProcess(NetMessage *msg);
        //determine play start time
        void playProcess(Json::Value *root, long *sec, long *nano);
        //process data list request
        void dataListProcess(Json::Value *root);

    private:
        //network connection to slave pi
        NetIO pi;
        //message handler for slave pi connection
        LimeMasterDownStream *downstream;

        //handle more commands
        void HandleMessageMore(NetMessage *msg, Json::Value* root);
        //handle commands before LimeShared
        bool HandleMessageEarly(NetMessage *msg, Json::Value* root);

        //is slave pi connected
        bool piConnected;
};

#endif // LIMEMASTER_H
