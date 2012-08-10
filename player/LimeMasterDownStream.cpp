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

#include "LimeMasterDownStream.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#include "LimeMaster.h"

LimeMasterDownStream::LimeMasterDownStream(LimeMaster *m, NetIO *net) {
    master = m;
    netIO = net;

    pthread_mutex_init(&m_lock, NULL);
    pthread_attr_setdetachstate(&m_tattr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_init(&m_tattr);
    m_thread    = 0;
    m_running   = false;
}

LimeMasterDownStream::~LimeMasterDownStream() {
    Stop();

    pthread_mutex_destroy(&m_lock);
    pthread_attr_destroy(&m_tattr);
}

bool LimeMasterDownStream::Start(){
    if(ThreadHandle())
        Stop();

    if(m_running) {
        FLog::Log(FLOG_DEBUG, "LimeMasterDownStream::Start - Message handler already running");
        return false;
    }

    m_running = true;

    pthread_create(&m_thread, &m_tattr, &LimeMasterDownStream::Run, this);

    FLog::Log(FLOG_DEBUG, "LimeMasterDownStream::Start - Thread with id %d started", (int)m_thread);
    return true;
}

bool LimeMasterDownStream::Stop() {
    if(!m_running) {
        FLog::Log(FLOG_DEBUG, "LimeMasterDownStream::Stop - Thread not running");
        return false;
    }

    pthread_join(m_thread, NULL);
    m_running = false;

    m_thread = 0;

    FLog::Log(FLOG_DEBUG, "LimeMasterDownStream::Stop - Stopped message handler");
    return true;
}

void *LimeMasterDownStream::Run(void *arg) {
    LimeMasterDownStream *thread = static_cast<LimeMasterDownStream *>(arg);
    thread->Process();

    FLog::Log(FLOG_DEBUG, "LimeMasterDownStream::Run - Finished message handling in thread with id %d\n", (int)thread->ThreadHandle());
    pthread_exit(NULL);
}

void LimeMasterDownStream::Process() {
    //handle messages, whilst we are running
    while(m_running && netIO->ThreadRunning()){
        //get next message
        NetMessage* msg = netIO->GetMessageQueue()->Pop(true);
        master->HandleMessageDown(msg);
    }
    m_running = false;
}
