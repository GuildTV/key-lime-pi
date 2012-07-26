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

#ifndef LIMETIMER_H
#define LIMETIMER_H

#define PLAYTIMEOUTMINUTES 2

#include "logger.h"
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <unistd.h>

#ifdef LIMEMASTER
class LimeMaster;
#else
class LimeSlave;
#endif

using namespace std;

class LimeTimer{
public:
#ifdef LIMEMASTER
    LimeTimer(LimeMaster *ms);
#else
    LimeTimer(LimeSlave *sl);
#endif
    virtual ~LimeTimer();
    pthread_t ThreadHandle();
    bool ThreadStop();
    bool ThreadRunning() {return running;};
    bool LoadGPIO();

    bool VideoPlay(long sec, long nano);

    void Lock();
    void UnLock();
protected:
    pthread_mutex_t     m_queue_lock;
    pthread_attr_t      m_tattr;
    struct sched_param  m_sched_param;
    pthread_t           m_thread;

private:
    static void *ThreadRun(void *arg);
    void ThreadProcess();
    bool running;
    pthread_mutex_t m_lock;
    long playSec;
    long playNano;

#ifdef LIMEMASTER
    LimeMaster *master;
#else
    LimeSlave  *slave;
#endif
};

#endif // LIMETIMER_H
