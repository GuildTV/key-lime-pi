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
    inValue == "";
    outHandle = NULL;
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
    if(immediate && type == GPIO_LOW && inValue.compare("0") == 0)
        return true;
    if(immediate && type == GPIO_HIGH && inValue.compare("1") == 0)
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
        usleep(pollRate);

        string input = ReadInput();
        Lock();
        if(input.compare(inValue) != 0) {
            inValue = input;

            if(inValue.compare("1") == 0){ //cond_high
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
    if(inValue.length() != 0){
        FLog::Log(FLOG_DEBUG, "GPIO::BindInput - Invalid starting value of inValue");
        return false;
    }

    if(pin < 0){
        FLog::Log(FLOG_DEBUG, "GPIO::BindInput - Invalid pin number");
        return false;
    }

    string prefix = "gpio";
    char name[21];
    sprintf(name, "%d", pin);
    string fullName = prefix + name;

    bool exp = ExportPin(pin);
    if(!exp){
        FLog::Log(FLOG_DEBUG, "GPIO::BindInput - Failed to export the pin");
        return false;
    }

    inPin = pin;
    pollRate = poll;

    string path = GPIOPATH;
    path += fullName;

    string direction = path + "/direction";
    string value = path + "/value";

    //write direction
    FILE* dirHandle = fopen(direction.c_str(), "w");
    fwrite("in", 1, 2, dirHandle);
    fclose(dirHandle);

    //read from gpio
    inValue = ReadInput();

    return true;
}

bool GPIO::BindOutput(int pin){
    if(outHandle)
        return false;

    if(pin < 0)
        return false;


    string prefix = "gpio";
    char name[21];
    sprintf(name, "%d", pin);
    string fullName = prefix + name;

    bool exp = ExportPin(pin);
    if(!exp)
        return false;

    outPin = pin;

    string path = GPIOPATH;
    path += fullName;

    string direction = path + "/direction";
    string value = path + "/value";

    //write direction
    FILE* dirHandle = fopen(direction.c_str(), "w");
    fwrite("out", 1, 3, dirHandle);
    fclose(dirHandle);

    //read from gpio
    outHandle = fopen(value.c_str(), "w");
    WriteOutput(GPIO_LOW);
    return true;
}

bool GPIO::UnBindInput() {
    if(inValue.length() != 0)
        return true;

    inValue = "";

    //unexport gpio pin
    return UnExportPin(outPin);
}

bool GPIO::UnBindOutput() {
    if(!outHandle)
        return true;

    //close file handle
    fclose(outHandle);

    //unexport gpio pin
    return UnExportPin(inPin);
}

bool GPIO::WriteOutput(GPIOState state) {
    if(!outHandle)
        return false;

    int val = int(state);
    char name[21];
    sprintf(name, "%d", val);
    FLog::Log(FLOG_DEBUG, "GPIO::WriteOutput - %d", val);
    rewind(outHandle);

    bool s = (fwrite(name, 1, 1, outHandle) == 1);
    fflush(outHandle);
    return s;
}

string GPIO::ReadInput() {
    char name[21];
    sprintf(name, "gpio%d", inPin);
    string exportCommand = "cat ";
    exportCommand += GPIOPATH;
    exportCommand += name;
    exportCommand += "/value";

    FILE* comm = popen(exportCommand.c_str(), "r");
    if(comm == NULL){
        FLog::Log(FLOG_ERROR, "GPIO::ReadInput - Failed to run command (%s)", exportCommand.c_str());
        pclose(comm);
        return "";
    }
    char buffer [2];
    if ( fgets (buffer , 2 , comm) == NULL ){
        FLog::Log(FLOG_ERROR, "GPIO::ReadInput - Invalid command return");
        pclose(comm);
        return "";
    }

    pclose(comm);
    return buffer;
}

bool GPIO::ExportPin(int pin) {
#ifndef GPIOTEST
    char name[21];
    sprintf(name, "%d", pin);
    string exportCommand = "gpio-admin export ";
    exportCommand += name;

    bool comRet = RunCommand(exportCommand.c_str(), true);
    if(!comRet) {
        FLog::Log(FLOG_ERROR, "GPIO::ExportPin - Failed to export pin (is GPIO-admin installed?)");
        return false;
    }
#endif

    return true;
}

bool GPIO::UnExportPin(int pin) {
#ifndef GPIOTEST
    char name[21];
    sprintf(name, "%d", pin);
    string exportCommand = "gpio-admin unexport ";
    exportCommand += name;

    bool comRet = RunCommand(exportCommand.c_str(), true);
    if(!comRet) {
        FLog::Log(FLOG_ERROR, "GPIO::UnExportPin - Failed to unexport pin (is GPIO-admin installed?)");
        return false;
    }
#endif

    return true;
}

bool GPIO::RunCommand(const char* command, bool silent){
    FILE* comm = popen(command, "r");
    if(comm == NULL){
        FLog::Log(FLOG_ERROR, "GPIO::RunCommand - Command returned null");
        pclose(comm);
        return false;
    }
    if(feof(comm)){
        FLog::Log(FLOG_ERROR, "GPIO::RunCommand - Unexpected end of command");
        pclose(comm);
        return false;
    }
    char buffer [100];
    if ( fgets (buffer , 100 , comm) != NULL ){
        FLog::Log(FLOG_ERROR, "GPIO::RunCommand - Invalid command return");
        pclose(comm);
        return false;
    }

    if(strlen(buffer) > 1 && silent){
        FLog::Log(FLOG_ERROR, "GPIO::RunCommand - Invalid command return");
        pclose(comm);
        return false;
    } else if (strlen(buffer) < 1 && !silent){
        FLog::Log(FLOG_ERROR, "GPIO::RunCommand - Invalid command return");
        pclose(comm);
        return false;
    }

    pclose(comm);
    return true;
}
