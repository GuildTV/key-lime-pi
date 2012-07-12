/*
 * NetIO.h
 *
 *  Created on: 10 Jul 2012
 *      Author: julus
 */

#ifndef NETIO_H_
#define NETIO_H_

#include <arpa/inet.h>
#include <sys/wait.h>
#include <netinet/tcp.h>

#include "NetUser.h"
#include "NetOpcode.h"

using namespace std;

class NetIO {
public:
	NetIO();
	~NetIO();
	int getSocketFD() { return thisSocketFD;};

	void Close();

    bool ThreadCreate();
	void ThreadProcess();
	bool ThreadRunning();
    pthread_t ThreadHandle();
    bool ThreadStop();

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
    void SendLock();
    void SendUnlock();

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
