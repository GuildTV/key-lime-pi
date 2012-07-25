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

#ifndef LIMESLAVE_H
#define LIMESLAVE_H

#include "net/NetIO.h"
#include <string>
#include "logger.h"
#include "GPIO.h"
#include "LimeGPIO.h"

#ifdef RENDERTEST
#include "render/OverlayRenderer.h"
#else
#include "OMXWrapper.h"
#endif

class LimeSlave
{
    public:
        LimeSlave();
        virtual ~LimeSlave();
        void Run();
        void Stop(){run=false;};
        void VideoStop();
        void VideoPlay();
        bool FinishSetup();
    protected:
        void VideoLoad(std::string name);
        bool FileExists(const char * filename);
        bool LoadGPIO();
    private:
        NetIO pi;
        bool run;
        void HandleMessage(NetMessage* msg);
        bool videoLoaded;
        LimeGPIO* limeGPIO;
#ifdef RENDERTEST
        OverlayRenderer* renderer;
#else
        OMXWrapper* wrap;
#endif
};

#endif // LIMESLAVE_H
