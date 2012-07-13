/*
 *
 *      Copyright (C) 2012 Edgar Hucek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

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

volatile bool     g_abort               = false;
bool              m_bMpeg               = false;
bool              m_Deinterlace         = false;
std::string       deviceString          = "omx:local";
bool              m_Pause               = false;
OMXReader         m_omx_reader;
bool              m_buffer_empty        = true;
bool              m_thread_player       = false;
OMXClock          *m_av_clock           = NULL;
COMXStreamInfo    m_hints_video;
OMXPacket         *m_omx_pkt            = NULL;
bool              m_hdmi_clock_sync     = false;
bool              m_stop                = false;
DllBcmHost        m_BcmHost;
OMXPlayerVideo    m_player_video;
int               m_tv_show_info        = 0;
bool              m_has_video           = false;
MyRender    m_my_render;

enum{ERROR=-1,SUCCESS,ONEBYTE};

static struct termios orig_termios;
static void restore_termios (int status, void * arg)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}

void sig_handler(int s)
{
  printf("strg-c catched\n");
  signal(SIGINT, SIG_DFL);
  g_abort = true;
}

void print_usage()
{
  printf("Usage: omxplayer [OPTIONS] [FILE]\n");
  printf("Options :\n");
  printf("         -h / --help                    print this help\n");
  printf("         -i / --info                    dump stream format and exit\n");
  printf("         -s / --stats                   pts and buffer stats\n");
  printf("         -d / --deinterlace             deinterlacing\n");
  printf("         -3 / --3d                      switch tv into 3d mode\n");
  printf("         -y / --hdmiclocksync           adjust display refresh rate to match video\n");
  printf("         -r / --refresh                 adjust framerate/resolution to video\n");
}

void SetSpeed(int iSpeed)
{
  if(!m_av_clock)
    return;

  if(iSpeed < OMX_PLAYSPEED_PAUSE)
    return;

  m_omx_reader.SetSpeed(iSpeed);

  if(m_av_clock->OMXPlaySpeed() != OMX_PLAYSPEED_PAUSE && iSpeed == OMX_PLAYSPEED_PAUSE)
    m_Pause = true;
  else if(m_av_clock->OMXPlaySpeed() == OMX_PLAYSPEED_PAUSE && iSpeed != OMX_PLAYSPEED_PAUSE)
    m_Pause = false;

  m_av_clock->OMXSpeed(iSpeed);
}

void FlushStreams(double pts)
{
//  if(m_av_clock)
//    m_av_clock->OMXPause();

  if(m_has_video)
    m_player_video.Flush();

  if(m_omx_pkt)
  {
    m_omx_reader.FreePacket(m_omx_pkt);
    m_omx_pkt = NULL;
  }

  if(pts != DVD_NOPTS_VALUE)
    m_av_clock->OMXUpdateClock(pts);

//  if(m_av_clock)
//  {
//    m_av_clock->OMXReset();
//    m_av_clock->OMXResume();
//  }
}

void SetVideoMode(int width, int height, float fps, bool is3d)
{
  int32_t num_modes;
  HDMI_RES_GROUP_T prefer_group;
  int i = 0;
  uint32_t prefer_mode;
  #define TV_MAX_SUPPORTED_MODES 60
  TV_SUPPORTED_MODE_T supported_modes[TV_MAX_SUPPORTED_MODES];
  uint32_t group = HDMI_RES_GROUP_CEA;

  if(is3d)
    group = HDMI_RES_GROUP_CEA_3D;
  else
    group = HDMI_RES_GROUP_CEA;

  num_modes = m_BcmHost.vc_tv_hdmi_get_supported_modes((HDMI_RES_GROUP_T)group,
                                          supported_modes, TV_MAX_SUPPORTED_MODES,
                                          &prefer_group, &prefer_mode);

  TV_SUPPORTED_MODE_T *tv_found = NULL;
  int ifps = (int)(fps+0.5f);
  //printf("num_modes %d, %d, %d\n", num_modes, prefer_group, prefer_mode);

  if (num_modes > 0 && prefer_group != HDMI_RES_GROUP_INVALID)
  {
    TV_SUPPORTED_MODE_T *tv = supported_modes;
    uint32_t best_score = 1<<30;
    uint32_t isNative = tv->native ? 1:0;
    uint32_t w = tv->width;
    uint32_t h = tv->height;
    uint32_t r = tv->frame_rate;
    uint32_t match_flag = HDMI_MODE_MATCH_FRAMERATE | HDMI_MODE_MATCH_RESOLUTION;
    uint32_t scan_mode = 0;
    uint32_t score = 0;

    for (i=0; i<num_modes; i++, tv++)
    {
      isNative = tv->native ? 1:0;
      w = tv->width;
      h = tv->height;
      r = tv->frame_rate;

      //printf("mode %dx%d@%d %s%s:%x\n", tv->width, tv->height,
      //       tv->frame_rate, tv->native?"N":"", tv->scan_mode?"I":"", tv->code);

      /* Check if frame rate match (equal or exact multiple) */
      if(ifps)
      {
        if(r == ((r/ifps)*ifps))
          score += abs((int)(r/ifps-1)) * (1<<8); // prefer exact framerate to multiples. Ideal is 1
        else
          score += ((match_flag & HDMI_MODE_MATCH_FRAMERATE) ? (1<<28):(1<<12))/r; // bad - but prefer higher framerate
      }
      /* Check size too, only choose, bigger resolutions */
      if(width && height)
      {
        /* cost of too small a resolution is high */
        score += max((int)(width - w), 0) * (1<<16);
        score += max((int)(height- h), 0) * (1<<16);
        /* cost of too high a resolution is lower */
        score += max((int)(w-width),   0) * ((match_flag & HDMI_MODE_MATCH_RESOLUTION) ? (1<<8):(1<<0));
        score += max((int)(h-height),  0) * ((match_flag & HDMI_MODE_MATCH_RESOLUTION) ? (1<<8):(1<<0));
      }
      else if (!isNative)
      {
        // native is good
        score += 1<<16;
      }

      if (scan_mode != tv->scan_mode)
        score += (match_flag & HDMI_MODE_MATCH_SCANMODE) ? (1<<20):(1<<8);

      if (w*9 != h*16) // not 16:9 is a small negative
        score += 1<<12;

      printf("mode %dx%d@%d %s%s:%x score=%d\n", tv->width, tv->height,
             tv->frame_rate, tv->native?"N":"", tv->scan_mode?"I":"", tv->code, score);

      if (score < best_score)
      {
        tv_found = tv;
        best_score = score;
      }
      /* reset score */
      score = 0;
    }
  }

  if(tv_found)
  {
    printf("Output mode %d: %dx%d@%d %s%s:%x\n", tv_found->code, tv_found->width, tv_found->height,
           tv_found->frame_rate, tv_found->native?"N":"", tv_found->scan_mode?"I":"", tv_found->code);
    // if we are closer to ntsc version of framerate, let gpu know
    int ifps = (int)(fps+0.5f);
    bool ntsc_freq = fabs(fps*1001.0f/1000.0f - ifps) < fabs(fps-ifps);
    printf("ntsc_freq:%d\n", ntsc_freq);
    char response[80];
    vc_gencmd(response, sizeof response, "hdmi_ntsc_freqs %d", ntsc_freq);
    m_BcmHost.vc_tv_hdmi_power_on_explicit(HDMI_MODE_HDMI, (HDMI_RES_GROUP_T)group, tv_found->code);
  }
}

int main(int argc, char *argv[])
{
  signal(SIGINT, sig_handler);

  struct termios new_termios;

  tcgetattr(STDIN_FILENO, &orig_termios);

  new_termios             = orig_termios;
  new_termios.c_lflag     &= ~(ICANON | ECHO | ECHOCTL | ECHONL);
  new_termios.c_cflag     |= HUPCL;
  new_termios.c_cc[VMIN]  = 0;

  std::string last_sub = "";

  tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
  on_exit(restore_termios, &orig_termios);

  std::string            m_filename;
  double                m_incr                = 0;
  CRBP                  g_RBP;
  COMXCore              g_OMX;
  bool                  m_stats               = false;
  bool                  m_dump_format         = false;
  bool                  m_3d                  = false;
  bool                  m_refresh             = false;
  double                startpts              = 0;
  TV_GET_STATE_RESP_T   tv_state;

  struct option longopts[] = {
    { "info",         no_argument,        NULL,          'i' },
    { "help",         no_argument,        NULL,          'h' },
    { "stats",        no_argument,        NULL,          's' },
    { "deinterlace",  no_argument,        NULL,          'd' },
    { "3d",           no_argument,        NULL,          '3' },
    { "hdmiclocksync", no_argument,       NULL,          'y' },
    { "refresh",      no_argument,        NULL,          'r' },
    { "sid",          required_argument,  NULL,          't' },
    { 0, 0, 0, 0 }
  };

  int c;
  while ((c = getopt_long(argc, argv, "wihn:o:cslpd3yt:rf:g:", longopts, NULL)) != -1)
  {
    switch (c)
    {
      case 'r':
        m_refresh = true;
        break;
      case 'y':
        m_hdmi_clock_sync = true;
        break;
      case '3':
        m_3d = true;
        break;
      case 'd':
        m_Deinterlace = true;
        break;
      case 's':
        m_stats = true;
        break;
      case 'o':
        deviceString = optarg;
        if(deviceString != "local" && deviceString != "hdmi")
        {
          print_usage();
          return 0;
        }
        deviceString = "omx:" + deviceString;
        break;
      case 'i':
        m_dump_format = true;
        break;
      case 0:
        break;
      case 'h':
        print_usage();
        return 0;
        break;
      case ':':
        return 0;
        break;
      default:
        return 0;
        break;
    }
  }

  if (optind >= argc) {
    print_usage();
    return 0;
  }

  m_filename = argv[optind];

  CLog::Init("./");

  g_RBP.Initialize();
  g_OMX.Initialize();

  m_av_clock = new OMXClock();

  m_thread_player = true;

  if(!m_omx_reader.Open(m_filename.c_str(), m_dump_format))
    goto do_exit;

  if(m_dump_format)
    goto do_exit;

  m_bMpeg         = m_omx_reader.IsMpegVideo();
  m_has_video     = m_omx_reader.VideoStreamCount();

  if(!m_av_clock->OMXInitialize(m_has_video, false))
    goto do_exit;

  if(m_hdmi_clock_sync && !m_av_clock->HDMIClockSync())
      goto do_exit;

  m_omx_reader.GetHints(OMXSTREAM_VIDEO, m_hints_video);

  if(m_has_video && !m_player_video.Open(m_hints_video, m_av_clock, m_Deinterlace,  m_bMpeg,
                                         m_hdmi_clock_sync, m_thread_player))
    goto do_exit;

  if(m_has_video && m_refresh)
  {
    memset(&tv_state, 0, sizeof(TV_GET_STATE_RESP_T));
    m_BcmHost.vc_tv_get_state(&tv_state);

    if(m_filename.find("3DSBS") != string::npos)
      m_3d = true;

    SetVideoMode(m_hints_video.width, m_hints_video.height, m_player_video.GetFPS(), m_3d);

  }

  m_my_render.Open(m_av_clock, m_thread_player);

  m_av_clock->SetSpeed(DVD_PLAYSPEED_NORMAL);
  m_av_clock->OMXStateExecute();
  m_av_clock->OMXStart();

  while(!m_stop)
  {
    int ch[8];
    int chnum = 0;

    if(g_abort)
      goto do_exit;

    while((ch[chnum] = getchar()) != EOF) chnum++;
    if (chnum > 1) ch[0] = ch[chnum - 1] | (ch[chnum - 2] << 8);

    switch(ch[0])
    {
      case 'z':
        m_tv_show_info = !m_tv_show_info;
        vc_tv_show_info(m_tv_show_info);
        break;
      case '1':
        SetSpeed(m_av_clock->OMXPlaySpeed() - 1);
        break;
      case '2':
        SetSpeed(m_av_clock->OMXPlaySpeed() + 1);
        break;
      case 'j':
        m_omx_reader.SetActiveStream(OMXSTREAM_AUDIO, m_omx_reader.GetAudioIndex() - 1);
        break;
      case 'k':
        m_omx_reader.SetActiveStream(OMXSTREAM_AUDIO, m_omx_reader.GetAudioIndex() + 1);
        break;
      case 'i':
        if(m_omx_reader.GetChapterCount() > 0)
        {
          m_omx_reader.SeekChapter(m_omx_reader.GetChapter() - 1, &startpts);
          FlushStreams(startpts);
        }
        else
        {
          m_incr = -600.0;
        }
        break;
      case 'o':
        if(m_omx_reader.GetChapterCount() > 0)
        {
          m_omx_reader.SeekChapter(m_omx_reader.GetChapter() + 1, &startpts);
          FlushStreams(startpts);
        }
        else
        {
          m_incr = 600.0;
        }
        break;
      case 'q':
        m_stop = true;
        goto do_exit;
        break;
      case 0x5b44: // key left
        if(m_omx_reader.CanSeek()) m_incr = -30.0;
        break;
      case 0x5b43: // key right
        if(m_omx_reader.CanSeek()) m_incr = 30.0;
        break;
      case 0x5b41: // key up
        if(m_omx_reader.CanSeek()) m_incr = 600.0;
        break;
      case 0x5b42: // key down
        if(m_omx_reader.CanSeek()) m_incr = -600.0;
        break;
      case ' ':
      case 'p':
        m_Pause = !m_Pause;
        if(m_Pause)
        {
          SetSpeed(OMX_PLAYSPEED_PAUSE);
          m_av_clock->OMXPause();
        }
        else
        {
          SetSpeed(OMX_PLAYSPEED_NORMAL);
          m_av_clock->OMXResume();
        }
        break;
      default:
        break;
    }

    if(m_Pause)
    {
      OMXClock::OMXSleep(2);
      continue;
    }

    if(m_incr != 0 && !m_bMpeg)
    {
      int    seek_flags   = 0;
      double seek_pos     = 0;
      double pts          = 0;

      m_av_clock->OMXStop();

      pts = m_av_clock->GetPTS();

      seek_pos = (pts / DVD_TIME_BASE) + m_incr;
      seek_flags = m_incr < 0.0f ? AVSEEK_FLAG_BACKWARD : 0;

      seek_pos *= 1000.0f;

      m_incr = 0;

      if(m_omx_reader.SeekTime(seek_pos, seek_flags, &startpts))
        FlushStreams(startpts);

      m_player_video.Close();
      if(m_has_video && !m_player_video.Open(m_hints_video, m_av_clock, m_Deinterlace,  m_bMpeg,
                                         m_hdmi_clock_sync, m_thread_player))
        goto do_exit;

      m_av_clock->OMXStart();
    }

    if(!m_omx_pkt)
      m_omx_pkt = m_omx_reader.Read();

    if(m_has_video && m_omx_pkt && m_omx_reader.IsActive(OMXSTREAM_VIDEO, m_omx_pkt->stream_index))
    {
      if(m_player_video.AddPacket(m_omx_pkt))
        m_omx_pkt = NULL;
      else
        OMXClock::OMXSleep(10);

      if(m_tv_show_info)
      {
        char response[80];
        vc_gencmd(response, sizeof response, "render_bar 4 video_fifo %d %d %d %d",
                m_player_video.GetDecoderBufferSize()-m_player_video.GetDecoderFreeSpace(),
                0 , 0, m_player_video.GetDecoderBufferSize());
      }
    }
    else
    {
      if(m_omx_pkt)
      {
        m_omx_reader.FreePacket(m_omx_pkt);
        m_omx_pkt = NULL;
      }
    }

    if(m_stats)
    {
      printf("V : %8.02f %8d %8d A : -- -- Cv : %8d Ca : --                            \r",
             m_player_video.GetCurrentPTS() / DVD_TIME_BASE, m_player_video.GetDecoderBufferSize(),
             m_player_video.GetDecoderFreeSpace(), m_player_video.GetCached());
    }
    if(m_omx_reader.IsEof())
        break;

  }

do_exit:
  printf("\n");

  if(!m_stop)
  {
	if(m_has_video)
      m_player_video.WaitCompletion();
  }

  if(m_refresh)
  {
    m_BcmHost.vc_tv_hdmi_power_on_best(tv_state.width, tv_state.height, tv_state.frame_rate, HDMI_NONINTERLACED,
                                       (EDID_MODE_MATCH_FLAG_T)(HDMI_MODE_MATCH_FRAMERATE|HDMI_MODE_MATCH_RESOLUTION|HDMI_MODE_MATCH_SCANMODE));
  }

  m_av_clock->OMXStop();
  m_av_clock->OMXStateIdle();

  m_player_video.Close();

  m_my_render.Close();

  if(m_omx_pkt)
  {
    m_omx_reader.FreePacket(m_omx_pkt);
    m_omx_pkt = NULL;
  }

  m_omx_reader.Close();

  vc_tv_show_info(0);

  g_OMX.Deinitialize();
  g_RBP.Deinitialize();

  printf("have a nice day ;)\n");
  return 1;
}