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

#include "LimeGPIO.h"

#ifdef LIMEMASTER
#include "LimeMaster.h"
#else
#include "LimeClient.h"
#endif

#define GPIO_TIMEOUT_SEC 1
#define GPIO_TIMEOUT_MILLI 100

#ifdef LIMEMASTER
LimeGPIO::LimeGPIO(LimeMaster *srv){
    server = srv;
    pthread_mutex_init(&m_lock, NULL);
}
#else
LimeGPIO::LimeGPIO(LimeClient *cl
    client = cl;
    pthread_mutex_init(&m_lock, NULL);
}
#endif

LimeGPIO::~LimeGPIO(){
    pthread_mutex_destroy(&m_lock);
}

bool LimeGPIO::LoadGPIO() {
    gpioHandle = new GPIO;

    bool in = gpioHandle->BindInput(GPIO_PLAY, 50000);//50ms
    bool out = gpioHandle->BindOutput(GPIO_READY);

    if(!in || !out)
        return false;

    if(!gpioHandle->WriteOutput(GPIO_LOW))
        return false;

    return true;
}

bool LimeGPIO::ThreadStop()
{
  running = false;

  pthread_join(m_thread, NULL);

  m_thread = 0;

  FLog::Log(FLOG_INFO, "LimeGPIO::%s - Thread stopped", __func__);
  return true;
}

void LimeGPIO::Lock(){
    pthread_mutex_lock(&m_lock);
}

void LimeGPIO::UnLock(){
    pthread_mutex_unlock(&m_lock);
}

bool LimeGPIO::VideoPlay()
{
  pthread_attr_init(&m_tattr);
  pthread_create(&m_thread, &m_tattr, &LimeGPIO::ThreadRun, this);

  running = true;

  FLog::Log(FLOG_INFO, "LimeGPIO::%s - Thread with id %d started", __func__, (int)m_thread);
  return true;
}

pthread_t LimeGPIO::ThreadHandle()
{
  return m_thread;
}

void *LimeGPIO::ThreadRun(void *arg)
{
  LimeGPIO *thread = static_cast<LimeGPIO *>(arg);
  thread->ThreadProcess();

  FLog::Log(FLOG_INFO, "LimeGPIO::%s - Exited thread with  id %d", __func__, (int)thread->ThreadHandle());
  pthread_attr_destroy(&(thread->m_tattr));
  pthread_exit(NULL);
}

void LimeGPIO::ThreadProcess() {
    FLog::Log(FLOG_INFO, "LimeGPIO::ThreadProcess - Watcher starting");
#ifdef LIMEMASTER
    //tell client to get ready
    if(!gpioHandle->WriteOutput(GPIO_HIGH)){
        FLog::Log(FLOG_DEBUG, "LimeGPIO::ThreadProcess (server) - Failed to send GPIO signal");
        return;
    }
    FLog::Log(FLOG_DEBUG, "LimeGPIO::ThreadProcess (server) - Sent GPIO signal");

    gpioHandle->ThreadCreate();

    //wait for clients response
    while(running){
        if(gpioHandle->CondWait(GPIO_HIGH, false, 0, 20)){
            FLog::Log(FLOG_DEBUG, "LimeGPIO::ThreadProcess (server) - Recieved GPIO signal");
            break;
        }
    }

    //start playing video here
    server->VideoPlay();

    //wait for client to say stop/end of video TODO


#else
    //wait for server to tell me to play
    while(running){
        if(gpioHandle->CondWait(GPIO_HIGH, false, 1, 0)){
            FLog::Log(FLOG_DEBUG, "LimeGPIO::ThreadProcess (client) - Recieved GPIO signal");
            break;
        }
    }

    //confirm
    gpioHandle->WriteOutput(GPIO_HIGH);
    FLog::Log(FLOG_DEBUG, "LimeGPIO::ThreadProcess (client) - Sent GPIO signal");

    //play
    client->VideoPlay();

    //wait for server to say stop/end of video TODO


#endif

    running = false;
    FLog::Log(FLOG_INFO, "LimeGPIO::ThreadProcess - Watcher stopped");
}
