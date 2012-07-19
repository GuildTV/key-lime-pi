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
}

MyRender::~MyRender()
{
  Close();

  pthread_mutex_destroy(&m_lock);
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


bool MyRender::Open(OMXClock *av_clock, bool use_thread)
{
  if (!av_clock)
    return false;

  if(ThreadHandle())
    Close();

  m_av_clock    = av_clock;
  m_use_thread  = use_thread;

  //m_FlipTimeStamp = m_av_clock->GetAbsoluteClock();
  printf("start?");

  if(m_use_thread)
    Create();

  return true;
}

bool MyRender::Close()
{
  m_bAbort  = true;

  if(ThreadHandle())
  {
    StopThread();
  }

  return true;
}


void MyRender::Process() {
	OverlayRenderer renderer;
	renderer.Create();

	double lastTime = 80000.0;
  while(!m_bStop && !m_bAbort){
	  double currentTime = m_av_clock->GetPTS();
	  if(currentTime != lastTime){
		  lastTime = currentTime;
		  //update here

	  }
		//render here??
		renderer.Draw();

  }
}
