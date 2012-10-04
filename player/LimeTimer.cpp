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

#include "LimeTimer.h"
#include "LimeShared.h"

LimeTimer::LimeTimer(LimeShared *sh){
    lime = sh;
    pthread_mutex_init(&m_lock, NULL);
}

LimeTimer::~LimeTimer(){
    pthread_mutex_destroy(&m_lock);
}

bool LimeTimer::ThreadStop()
{
  running = false;

  pthread_join(m_thread, NULL);

  m_thread = 0;

  FLog::Log(FLOG_INFO, "LimeTimer::%s - Thread stopped", __func__);
  return true;
}

void LimeTimer::Lock(){
    pthread_mutex_lock(&m_lock);
}

void LimeTimer::UnLock(){
    pthread_mutex_unlock(&m_lock);
}

bool LimeTimer::VideoPlay(string scr, long sec, long nano)
{
  lime->getNetUp().GetClient()->SendMessage("{\"type\":\"playVideo\",\"status\":\"waiting for playback\"}");

  pthread_attr_init(&m_tattr);
  pthread_create(&m_thread, &m_tattr, &LimeTimer::ThreadRun, this);

  running = true;
  playNano = nano;
  playSec = sec;
  script = scr;

  FLog::Log(FLOG_INFO, "LimeTimer::%s - Thread with id %d started", __func__, (int)m_thread);
  return true;
}

pthread_t LimeTimer::ThreadHandle()
{
  return m_thread;
}

void *LimeTimer::ThreadRun(void *arg)
{
  LimeTimer *thread = static_cast<LimeTimer *>(arg);
  thread->ThreadProcess();

  FLog::Log(FLOG_INFO, "LimeTimer::%s - Exited thread with  id %d", __func__, (int)thread->ThreadHandle());
  pthread_attr_destroy(&(thread->m_tattr));
  pthread_exit(NULL);
}

void LimeTimer::ThreadProcess() {
    FLog::Log(FLOG_INFO, "LimeTimer::ThreadProcess - Watcher starting");

    //create timespec
    timespec ts;
    //get time
    clock_gettime(CLOCK_REALTIME, &ts);

    //wait until time is after time specified earlier
    while(ts.tv_sec <= playSec && ts.tv_nsec < playNano){
        clock_gettime(CLOCK_REALTIME, &ts);
    }
    //call back out to play video
    lime->VideoPlay(script);
    running = false;
    FLog::Log(FLOG_INFO, "LimeTimer::ThreadProcess - Watcher stopped");
}
