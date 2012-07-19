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

#include "MyRender.h"

#include <string>

static struct termios orig_termios;
static void restore_termios (int status, void * arg)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

class OMXLink
{
    public:
        OMXLink();
        virtual ~OMXLink();
        int Run();
        void SetSpeed(int iSpeed);
        void FlushStreams(double pts);
    protected:
    private:
        volatile bool     g_abort;
bool              m_bMpeg         ;
bool              m_Deinterlace     ;
bool              m_Pause            ;
OMXReader         m_omx_reader;
bool              m_buffer_empty      ;
bool              m_thread_player   ;
OMXClock          *m_av_clock      ;
COMXStreamInfo    m_hints_video;
OMXPacket         *m_omx_pkt     ;
bool              m_stop            ;
DllBcmHost        m_BcmHost;
OMXPlayerVideo    m_player_video;
bool              m_has_video        ;
MyRender    m_my_render;



};

#endif // OMXLINK_H
