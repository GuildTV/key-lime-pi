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

#ifndef GPIO_H
#define GPIO_H

#include "logger.h"
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <bcm2835.h>
using namespace std;

enum GPIOState { GPIO_LOW=0, GPIO_HIGH=1 };

class GPIO{
public:
    GPIO();
    virtual ~GPIO();
    bool ThreadCreate();
    pthread_t ThreadHandle();
    bool ThreadStop();
    bool ThreadRunning() {return running;};

    bool BindInput(int port, int poll);
    void UnBindInput();
    bool BindOutput(int port);
    void UnBindOutput();

    int ReadInput();
    bool WriteOutput(GPIOState state);
    void SetPollTime(long poll);

    void Lock();
    void UnLock();
    bool CondWait(GPIOState type, bool immediate, long seconds, long milli);
protected:
    pthread_mutex_t     m_queue_lock;
    pthread_attr_t      m_tattr;
    struct sched_param  m_sched_param;
    pthread_t           m_thread;

private:
    static void *ThreadRun(void *arg);
    void ThreadProcess();
    bool running;
    int inPin;
    int outPin;
    int inValue;
    long pollRate;
    pthread_mutex_t m_lock;
    pthread_cond_t m_cond_high;
    pthread_cond_t m_cond_low;
    int lowCount;
    int highCount;


};

#endif // GPIO_H
