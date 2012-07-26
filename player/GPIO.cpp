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

#include "GPIO.h"


#ifdef GPIOTEST
#define GPIOPATH "dummy-gpio/"
#else
#define GPIOPATH "/sys/devices/virtual/gpio/"
#endif

GPIO::GPIO(){
    bcm2835_init();
    inValue = -1;
    inPin = -1;
    outPin = -1;
    pthread_mutex_init(&m_lock, NULL);
    pthread_cond_init(&m_cond_high, NULL);
    pthread_cond_init(&m_cond_low, NULL);
    lowCount = 0;
    highCount = 0;
}

GPIO::~GPIO(){
    UnBindInput();
    pthread_mutex_destroy(&m_lock);
    pthread_cond_destroy(&m_cond_high);
    pthread_cond_destroy(&m_cond_low);
    bcm2835_close();
}


bool GPIO::ThreadStop()
{
  running = false;

  pthread_join(m_thread, NULL);

  m_thread = 0;

  FLog::Log(FLOG_INFO, "GPIO::%s - Thread stopped", __func__);
  return true;
}

void GPIO::Lock(){
    pthread_mutex_lock(&m_lock);
}

void GPIO::UnLock(){
    pthread_mutex_unlock(&m_lock);
}

bool GPIO::CondWait(GPIOState type, bool immediate, long seconds, long milli){
    if(immediate && type == GPIO_LOW && inValue == 0)
        return true;
    if(immediate && type == GPIO_HIGH && inValue == 1)
        return true;

    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += seconds;
    ts.tv_nsec += milli*1000;

    Lock();

    int ret;
    if(type == GPIO_HIGH) {
        highCount++;
        ret = pthread_cond_timedwait(&m_cond_high, &m_lock, &ts);
        highCount--;
    } else {
        lowCount++;
        ret = pthread_cond_timedwait(&m_cond_low, &m_lock, &ts);
        lowCount--;
    }

    UnLock();

    if(ret == 0)
        return true;
    else if(ret == ETIMEDOUT)
        return false;

    FLog::Log(FLOG_INFO, "GPIO::CondWaitHigh - Timed wait threw an error (%d)", ret);
    return false;
}

bool GPIO::ThreadCreate()
{
  pthread_attr_init(&m_tattr);
  pthread_create(&m_thread, &m_tattr, &GPIO::ThreadRun, this);

  running = true;

  FLog::Log(FLOG_INFO, "GPIO::%s - Thread with id %d started", __func__, (int)m_thread);
  return true;
}

pthread_t GPIO::ThreadHandle()
{
  return m_thread;
}

void *GPIO::ThreadRun(void *arg)
{
  GPIO *thread = static_cast<GPIO *>(arg);
  thread->ThreadProcess();

  FLog::Log(FLOG_INFO, "GPIO::%s - Exited thread with  id %d", __func__, (int)thread->ThreadHandle());
  pthread_attr_destroy(&(thread->m_tattr));
  pthread_exit(NULL);
}

void GPIO::ThreadProcess() {
    FLog::Log(FLOG_INFO, "GPIO::ThreadProcess - Watcher starting");
    while(running){
        delay(pollRate);

        int input = ReadInput();
        Lock();
        if(input == inValue) {
            inValue = input;

            if(inValue == 1){ //cond_high
                if(highCount == 0) {
                } else if(highCount == 1)
                    pthread_cond_signal(&m_cond_high);
                else
                    pthread_cond_broadcast(&m_cond_high);
            } else { //cond_low
                if(lowCount == 0) {
                } else if(lowCount == 1)
                    pthread_cond_signal(&m_cond_low);
                else
                    pthread_cond_broadcast(&m_cond_low);
            }
        }
        UnLock();
    }

    running = false;
    FLog::Log(FLOG_INFO, "GPIO::ThreadProcess - Watcher stopped");
}

void GPIO::SetPollTime(long poll) {
    pollRate = poll;
}

bool GPIO::BindInput(int pin, int poll){
    if(inValue != -1){
        FLog::Log(FLOG_DEBUG, "GPIO::BindInput - Invalid starting value of inValue");
        return false;
    }

    if(inPin > 0){
        FLog::Log(FLOG_DEBUG, "GPIO::BindInput - Trying to bind input whilst one is already bound");
    }

    if(pin < 0){
        FLog::Log(FLOG_DEBUG, "GPIO::BindInput - Invalid pin number");
        return false;
    }

    inPin = pin;

    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(pin, BCM2835_GPIO_PUD_UP);

    //read from gpio
    inValue = ReadInput();

    return true;
}

bool GPIO::BindOutput(int pin){
    if(outPin != -1){
        FLog::Log(FLOG_DEBUG, "GPIO::BindOutput - Trying to bind input whilst one is already bound");
    }

    if(pin < 0)
        return false;


    outPin = pin;

    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_set_pud(pin, BCM2835_GPIO_PUD_UP);

    WriteOutput(GPIO_LOW);
    return true;
}

void GPIO::UnBindInput() {
    inPin = -1;
    inValue = -1;
}

void GPIO::UnBindOutput() {
    outPin = -1;
}

bool GPIO::WriteOutput(GPIOState state) {
    if(outPin == -1)
        return false;

    int val = int(state);

    bcm2835_gpio_write(outPin, val);

    return true;
}

int GPIO::ReadInput() {
    if(inPin == 0){
        FLog::Log(FLOG_DEBUG, "GPIO::ReadInput - trying to read input without a valid pin being bound");
        return -1;
    }

    return bcm2835_gpio_lev(inPin);
}
