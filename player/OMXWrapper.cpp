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

#include "OMXWrapper.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

OMXWrapper::OMXWrapper()
{
pthread_mutex_init(&m_lock, NULL);
  pthread_attr_setdetachstate(&m_tattr, PTHREAD_CREATE_JOINABLE);
  pthread_attr_init(&m_tattr);
  m_thread    = 0;
  m_running   = false;
}

OMXWrapper::~OMXWrapper()
{
  Stop();

  pthread_mutex_destroy(&m_lock);
  pthread_attr_destroy(&m_tattr);
}

bool OMXWrapper::Play()
{
  if(ThreadHandle())
    Stop();

  if(m_running)
  {
    FLog::Log(FLOG_DEBUG, "OMXWrapper::Play - Video is already playing");
    return false;
  }

  m_running = true;

  pthread_create(&m_thread, &m_tattr, &OMXWrapper::Run, this);

  FLog::Log(FLOG_DEBUG, "OMXWrapper::Play - Thread with id %d started", (int)m_thread);
  return true;
}

bool OMXWrapper::Stop()
{
  if(!m_running)
  {
    FLog::Log(FLOG_DEBUG, "OMXWrapper::Stop - No video playing");
    return false;
  }

  omx->Exit();

  pthread_join(m_thread, NULL);
  m_running = false;

  m_thread = 0;

  FLog::Log(FLOG_DEBUG, "OMXWrapper::Stop - Stopped Video Playback");
  return true;
}

bool OMXWrapper::Running()
{
  return m_running;
}

pthread_t OMXWrapper::ThreadHandle()
{
  return m_thread;
}

void *OMXWrapper::Run(void *arg)
{
  OMXWrapper *thread = static_cast<OMXWrapper *>(arg);
  thread->Process();

  FLog::Log(FLOG_DEBUG, "OMXWrapper::Run - Finished playback in thread with id %d\n", (int)thread->ThreadHandle());
  pthread_exit(NULL);
}

void OMXWrapper::Load(string file) {
    omx = new OMXLink;
    omx->Load(file);
}

void OMXWrapper::Process() {
    omx->Play();
}
