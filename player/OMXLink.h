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

#ifndef OMXLINK_H
#define OMXLINK_H

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <stdint.h>
#include <termios.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <getopt.h>

#define AV_NOWARN_DEPRECATED

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
};

#include "OMX/OMXStreamInfo.h"

#include "utils/log.h"
#include "logger.h"

#include "OMX/DllAvUtil.h"
#include "OMX/DllAvFormat.h"
#include "OMX/DllAvFilter.h"
#include "OMX/DllAvCodec.h"
#include "linux/RBP.h"

#include "OMX/OMXVideo.h"
#include "OMX/OMXClock.h"
#include "OMX/OMXReader.h"
#include "OMX/OMXPlayerVideo.h"
#include "OMX/DllOMX.h"

#include "render/MyRender.h"

#include <string>

class OMXLink
{
    public:
        OMXLink();
        ~OMXLink();
        int Load(std::string m_filename);
        int Play();
        int Exit();
        void SetSpeed(int iSpeed);
        void FlushStreams(double pts);

    protected:
    private:
        static void *ThreadRun(void *arg);

        volatile bool     g_abort;
        bool              m_bMpeg;
        bool              m_Deinterlace;
        bool              m_Pause;
        OMXReader         m_omx_reader;
        bool              m_buffer_empty;
        bool              m_thread_player;
        OMXClock          *m_av_clock;
        COMXStreamInfo    m_hints_video;
        OMXPacket         *m_omx_pkt;
        bool              m_stop;
        DllBcmHost        m_BcmHost;
        OMXPlayerVideo    m_player_video;
        bool              m_has_video;
        MyRender    m_my_render;

        std::string filename;
        CRBP                  *g_RBP;
        COMXCore              *g_OMX;
};

#endif // OMXLINK_H
