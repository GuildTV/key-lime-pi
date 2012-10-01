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

MyRender::MyRender(NetIO *net)
{
pthread_mutex_init(&m_lock, NULL);
  pthread_attr_setdetachstate(&m_tattr, PTHREAD_CREATE_JOINABLE);
  pthread_attr_init(&m_tattr);
  m_thread    = 0;
  m_bStop     = false;
  m_running   = false;

  netIO = net;
  renderer = new OverlayRenderer(netIO);
}

MyRender::~MyRender()
{
  Close();

  pthread_mutex_destroy(&m_lock);
  pthread_attr_destroy(&m_tattr);
}

void MyRender::Lock()
{
    pthread_mutex_lock(&m_lock);
}

void MyRender::UnLock()
{
    pthread_mutex_unlock(&m_lock);
}

#ifndef RENDERTEST
bool MyRender::Play(OMXClock *av_clock, OMXPlayerVideo *m_player_video) {
#else
bool MyRender::Play() {
#endif

#ifndef RENDERTEST
  if (!av_clock)
    return false;
#endif

  if(ThreadHandle())
    Close();

#ifndef RENDERTEST
  m_av_clock    = av_clock;
  vid = m_player_video;
#endif

  if(m_running)
  {
    FLog::Log(FLOG_ERROR, "MyRender::%s - Thread already running\n", __func__);
    return false;
  }

  m_bStop    = false;
  m_running = true;

  pthread_create(&m_thread, &m_tattr, &MyRender::Run, this);

  FLog::Log(FLOG_DEBUG, "MyRender::%s - Thread with id %d started\n", __func__, (int)m_thread);
  return true;
}

bool MyRender::Close()
{
  if(!m_running)
  {
    FLog::Log(FLOG_DEBUG, "MyRender::%s - No thread running\n", __func__);
    return false;
  }

  if(renderer){
    renderer->Stop();
  }

  m_bStop = true;
  pthread_join(m_thread, NULL);
  m_running = false;

  m_thread = 0;

  FLog::Log(FLOG_DEBUG, "MyRender::%s - Thread stopped\n", __func__);
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

  FLog::Log(FLOG_DEBUG, "MyRender::%s - Exited thread with  id %d\n", __func__, (int)thread->ThreadHandle());
  pthread_exit(NULL);
}

void MyRender::Create(std::string file, Json::Value data){
  //create the renderer
  renderer->Create(file, data);

#ifdef LIMEMASTER
    //draw prevideo frame
    renderer->PreDraw();
#else
    //unbind context
    renderer->unBind();
#endif
}

void MyRender::Process() {
#ifndef RENDERTEST
  //define the start time of the video
  double startTime = 2840000.0; // currently vid->GetCurrentPTS*1.75??

  //loop until start time is reached
  while(!m_bStop && m_running){
	  double currentTime = vid->GetCurrentPTS();
	  if(currentTime > startTime){
	      //start rendering
		  renderer->Run();
		  break;
	  }
  }
#else
    //start rendering
    renderer->Run();
#endif

  m_running = false;
}
