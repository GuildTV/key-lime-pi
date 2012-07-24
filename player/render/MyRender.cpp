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

#include "MyRender.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

MyRender::MyRender()
{
pthread_mutex_init(&m_lock, NULL);
  pthread_attr_setdetachstate(&m_tattr, PTHREAD_CREATE_JOINABLE);
  pthread_attr_init(&m_tattr);
  m_thread    = 0;
  m_bStop     = false;
  m_running   = false;
}

MyRender::~MyRender()
{
  Close();

  pthread_mutex_destroy(&m_lock);
  pthread_attr_destroy(&m_tattr);
}

void MyRender::Lock()
{
  if(m_use_thread)
    pthread_mutex_lock(&m_lock);
}

void MyRender::UnLock()
{
  if(m_use_thread)
    pthread_mutex_unlock(&m_lock);
}


bool MyRender::Open(OMXClock *av_clock, bool use_thread, OMXPlayerVideo *m_player_video, std::string file)
{
  if (!av_clock)
    return false;

  if(ThreadHandle())
    Close();

  m_av_clock    = av_clock;
  m_use_thread  = use_thread;
  vid = m_player_video;
  filename = file;

  //m_FlipTimeStamp = m_av_clock->GetAbsoluteClock();
  printf("start?");

  if(m_running)
  {
    CLog::Log(LOGERROR, "MyRender::%s - Thread already running\n", __func__);
    return false;
  }

  m_bStop    = false;
  m_running = true;

  pthread_create(&m_thread, &m_tattr, &MyRender::Run, this);

  CLog::Log(LOGDEBUG, "MyRender::%s - Thread with id %d started\n", __func__, (int)m_thread);
  return true;
}

bool MyRender::Close()
{
  if(!m_running)
  {
    CLog::Log(LOGDEBUG, "MyRender::%s - No thread running\n", __func__);
    return false;
  }

  if(renderer){
    renderer->Stop();
  }

  m_bStop = true;
  pthread_join(m_thread, NULL);
  m_running = false;

  m_thread = 0;

  CLog::Log(LOGDEBUG, "MyRender::%s - Thread stopped\n", __func__);
  return true;
}

bool MyRender::Running()
{
  return m_running;
}

pthread_t MyRender::ThreadHandle()
{
  return m_thread;
}

void *MyRender::Run(void *arg)
{
  MyRender *thread = static_cast<MyRender *>(arg);
  thread->Process();

  CLog::Log(LOGDEBUG, "MyRender::%s - Exited thread with  id %d\n", __func__, (int)thread->ThreadHandle());
  pthread_exit(NULL);
}


void MyRender::Process() {

  renderer = new OverlayRenderer(filename);
  renderer->PreDraw();
  double startTime = 2840000.0; // currently vid->GetCurrentPTS*1.75??

  while(!m_bStop && m_running){
	  double currentTime = vid->GetCurrentPTS();
	  if(currentTime > startTime){
		  renderer->Run();
		  break;

	  }

  }
}
