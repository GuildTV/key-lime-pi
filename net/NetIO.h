/*
 * NetIO.h
 *
 *  Created on: 10 Jul 2012
 *      Author: julus
 */

#ifndef NETIO_H_
#define NETIO_H_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <pthread.h>
#include <queue>
#include <sys/wait.h>
#include <signal.h>

#include "NetUser.h"
#include "NetOpcode.h"


using namespace std;

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

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

    int CreateServer(char port[]);

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
	NetUser slave;

    NetMessageQueue messageQueue;
};

#endif /* NETIO_H_ */
