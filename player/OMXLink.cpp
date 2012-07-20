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

#include "OMXLink.h"

OMXLink::OMXLink() {
    g_abort               = false;
    m_bMpeg               = false;
    m_Deinterlace         = false;
    m_Pause               = false;
    m_buffer_empty        = true;
    m_thread_player       = false;
    m_av_clock           = NULL;
    m_omx_pkt            = NULL;
    m_stop                = false;
    m_has_video           = false;
}

OMXLink::~OMXLink()
{
}

void OMXLink::SetSpeed(int iSpeed)
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

void OMXLink::FlushStreams(double pts)
{
  if(m_has_video)
    m_player_video.Flush();

  if(m_omx_pkt)
  {
    m_omx_reader.FreePacket(m_omx_pkt);
    m_omx_pkt = NULL;
  }

  if(pts != DVD_NOPTS_VALUE)
    m_av_clock->OMXUpdateClock(pts);
}

int OMXLink::Load(std::string m_filename) {
    FLog::Log(FLOG_INFO, "OMXLink::%s - GO", __func__);

    printf("\nrun - %s %d\n", filename.c_str(), filename.length());
    printf("run - %s %d\n", filename.c_str(), filename.length());

printf("o - %s\n", filename.c_str());
  g_RBP = new CRBP;
  g_OMX = new COMXCore;
printf("o - %s\n", filename.c_str());

  //m_filename = "data/new/video.mp4";
  //m_filename = filename.c_str();
filename = m_filename;
  CLog::Init("./");
printf("o - %s\n", filename.c_str());
  g_RBP->Initialize();
  g_OMX->Initialize();

  m_av_clock = new OMXClock();

  m_thread_player = true;
 // m_filename = filename;
  printf("o - %s\n", filename.c_str());

  if(!m_omx_reader.Open(filename.c_str(), false)){
    Exit();
    return -1;
  }

  m_bMpeg         = m_omx_reader.IsMpegVideo();
  m_has_video     = m_omx_reader.VideoStreamCount();

  if(!m_av_clock->OMXInitialize(m_has_video, false)){
    Exit();
    return -1;
  }

  m_omx_reader.GetHints(OMXSTREAM_VIDEO, m_hints_video);

  if(m_has_video && !m_player_video.Open(m_hints_video, m_av_clock, m_Deinterlace,  m_bMpeg,
                                         false, m_thread_player)){
    Exit();
    return -1;
  }

  m_my_render.Open(m_av_clock, m_thread_player, &m_player_video, filename);

  m_av_clock->SetSpeed(DVD_PLAYSPEED_NORMAL);
  m_av_clock->OMXStateExecute();
  m_av_clock->OMXStart();

  SetSpeed(OMX_PLAYSPEED_PAUSE);
  m_av_clock->OMXPause();

  OMXClock::OMXSleep(5000);

  return 1;
}

int OMXLink::Play() {
  SetSpeed(OMX_PLAYSPEED_NORMAL);
  m_av_clock->OMXResume();

  while(!m_stop)
  {
    if(g_abort){
    Exit();
    return -1;
  }

    if(m_Pause)
    {
      OMXClock::OMXSleep(2);
      continue;
    }

    if(!m_omx_pkt)
      m_omx_pkt = m_omx_reader.Read();

    if(m_has_video && m_omx_pkt && m_omx_reader.IsActive(OMXSTREAM_VIDEO, m_omx_pkt->stream_index))
    {
      if(m_player_video.AddPacket(m_omx_pkt))
        m_omx_pkt = NULL;
      else
        OMXClock::OMXSleep(10);
    }
    else
    {
      if(m_omx_pkt)
      {
        m_omx_reader.FreePacket(m_omx_pkt);
            m_omx_pkt = NULL;
      }
    }

    if(m_omx_reader.IsEof())
        break;

  }

  Exit();
  return 1;
}

int OMXLink::Exit() {
  printf("\n");

  if(!m_stop)
  {
	if(m_has_video)
      m_player_video.WaitCompletion();
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

  g_OMX->Deinitialize();
  g_RBP->Deinitialize();

  return 1;
}
/*
int main (int argc, char *argv[]){
    OMXLink omx;
    omx.ThreadCreate();
    while(true){}
    return 1;
}
*/
