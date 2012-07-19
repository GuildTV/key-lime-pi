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

#ifndef NETIO_H_
#define NETIO_H_

#include <arpa/inet.h>
#include <sys/wait.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <unistd.h>

#include "logger.h"

#include "NetUser.h"
#include "NetOpcode.h"

using namespace std;

class NetIO {
public:
	NetIO();
	~NetIO();
	int getSocketFD() { return thisSocketFD;};

	void Close();

	bool ThreadRunning();
    pthread_t ThreadHandle();

    int CreateServer(string port);
    int CreateClient(string address, string port);

    NetUser* GetClient() {return &client;};
    NetMessageQueue* GetMessageQueue() {return &messageQueue;};

protected:
    pthread_mutex_t     m_queue_lock;
    pthread_attr_t      m_tattr;
    struct sched_param  m_sched_param;
    pthread_t           m_thread;
    volatile bool       m_running;
    volatile bool       m_bStop;

    bool ThreadCreate();
	void ThreadProcess();
    bool ThreadStop();

private:
    static void *ThreadRun(void *arg);

	int thisSocketFD;
	struct addrinfo *servinfo;
	socklen_t addr_size;
	NetUser client;
	NetRole role;

    NetMessageQueue messageQueue;
};

#endif /* NETIO_H_ */
