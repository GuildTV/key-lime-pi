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

#ifndef LIMEGPIO_H
#define LIMEGPIO_H

#ifndef GPIO_READY
#define GPIO_READY 21
#endif
#ifndef GPIO_PLAY
#define GPIO_PLAY 22
#endif


#ifndef LIMEMASTER
#ifndef LIMESLAVE
#define LIMEMASTER
#endif
#endif

#include "GPIO.h"
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

class LimeGPIO{
public:
#ifdef LIMEMASTER
    LimeGPIO(LimeMaster *srv);
#else
    LimeGPIO(LimeSlave *cl);
#endif
    virtual ~LimeGPIO();
    pthread_t ThreadHandle();
    bool ThreadStop();
    bool ThreadRunning() {return running;};
    bool LoadGPIO();

    bool VideoPlay();

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
    GPIO *gpioHandle;

#ifdef LIMEMASTER
    LimeMaster *server;
#else
    LimeSlave  *client;
#endif
};

#endif // LIMEGPIO_H
