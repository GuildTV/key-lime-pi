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

#include "NetIO.h"
#include <netdb.h>

//backlog of clients waiting to connect to the server
#define BACKLOG 5

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

NetIO::NetIO() {
    pthread_mutex_init(&m_queue_lock, NULL);
    pthread_attr_setdetachstate(&m_tattr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_init(&m_tattr);
    m_thread    = 0;
    m_bStop     = false;
    m_running   = false;
    role = UNDEFINED;
}

NetIO::~NetIO() {
    pthread_mutex_destroy(&m_queue_lock);
    pthread_attr_destroy(&m_tattr);
}

bool NetIO::ThreadStop()
{
  if(!m_running)
  {
    FLog::Log(FLOG_ERROR, "NetIO::ThreadStop - No thread running");
    return false;
  }

  m_bStop = true;
  pthread_join(m_thread, NULL);
  m_running = false;

  m_thread = 0;

  //printf("NetIO::%s - Thread stopped\n", __func__);
  return true;
}

bool NetIO::ThreadCreate()
{
  if(m_running)
  {
   FLog::Log(FLOG_ERROR, "NetIO::ThreadCreate - Thread already running");
    return false;
  }

  m_bStop    = false;
  m_running = true;

  pthread_create(&m_thread, &m_tattr, &NetIO::ThreadRun, this);

  //printf("NetIO::%s - Thread with id %d started\n", __func__, (int)m_thread);
  return true;
}

void *NetIO::ThreadRun(void *arg)
{
  NetIO *thread = static_cast<NetIO *>(arg);
  thread->ThreadProcess();

  //printf("NetIO::%s - Exited thread with  id %d\n", __func__, (int)thread->ThreadHandle());
  pthread_exit(NULL);
}

void NetIO::Close() {
    FLog::Log(FLOG_INFO, "NetIO::Close - Sockets closed");
    //close and tidy up this
    close(thisSocketFD);
    client.Close();
    role = UNDEFINED;
}

int NetIO::CreateServer(string port) {
    struct addrinfo hints, *servinfo, *p;
    struct sigaction sa;
    int yes=1;
    int rv;

    //setup bind address and port
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, port.c_str(), &hints, &servinfo)) != 0) {
        FLog::Log(FLOG_INFO, "NetIO::CreateServer - getaddrinfo: %s", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((thisSocketFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            FLog::Log(FLOG_DEBUG, "NetIO::CreateServer - Failed to create socket");
            continue;
        }

        if (setsockopt(thisSocketFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            FLog::Log(FLOG_DEBUG, "NetIO::CreateServer - Failed to set socket as reusable");
           continue;
        }

        if (setsockopt(thisSocketFD, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(int)) == -1) {
            FLog::Log(FLOG_DEBUG, "NetIO::CreateServer - Failed to set socket flags");
            continue;
        }

        if (::bind(thisSocketFD, p->ai_addr, p->ai_addrlen) == -1) {
            close(thisSocketFD);
            FLog::Log(FLOG_DEBUG, "NetIO::CreateServer - Failed to bind socket");
            continue;
        }

        break;
    }

    if (p == NULL)  {
        FLog::Log(FLOG_ERROR, "NetIO::CreateServer - Failed to bind a socket");
        return 2;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (listen(thisSocketFD, BACKLOG) == -1) {
        FLog::Log(FLOG_ERROR, "NetIO::CreateServer - Failed to start listening");
        return 3;
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        FLog::Log(FLOG_ERROR, "NetIO::CreateServer - Sigaction error");
        return 4;
    }

    //define as server
    role = SERVER;
    //start listening for clients/messages
    ThreadCreate();

    return 0;
}

int NetIO::CreateClient(string address, string port) {
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    //define address and port
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(address.c_str(), port.c_str(), &hints, &servinfo)) != 0) {
        FLog::Log(FLOG_INFO, "NetIO::CreateClient - getaddrinfo: %s", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((thisSocketFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            FLog::Log(FLOG_DEBUG, "NetIO::CreateClient - Failed to create socket");
            continue;
        }

        if (connect(thisSocketFD, p->ai_addr, p->ai_addrlen) == -1) {
            close(thisSocketFD);
            FLog::Log(FLOG_DEBUG, "NetIO::CreateClient - Failed to connect to server");
            continue;
        }

        break;
    }

    if (p == NULL) {
        FLog::Log(FLOG_DEBUG, "NetIO::CreateClient - Failed to connect to %s:%s", address.c_str(), port.c_str());
        return 2;
    }

    //get and log the server address
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    FLog::Log(FLOG_INFO, "NetIO::CreateClient - Connecting to %s:", s);

    freeaddrinfo(servinfo); // all done with this structure

    //create this client
    client.Create(thisSocketFD, &messageQueue, CLIENT);

    //define role
    role = CLIENT;
    //start listening for messages
    ThreadCreate();

    return 0;
}

void NetIO::ThreadProcess() {
    if(role == SERVER) {
        struct sockaddr_storage their_addr; // connector's address information
        socklen_t sin_size;
        char s[INET6_ADDRSTRLEN];
        int new_fd;

        while(!m_bStop && m_running) {
            //wait for client
            FLog::Log(FLOG_INFO, "NetIO::ThreadProcess(Server) - Waiting for connections....");
            sin_size = sizeof their_addr;
            new_fd = accept(thisSocketFD, (struct sockaddr *)&their_addr, &sin_size);
            if (new_fd == -1) {
                   FLog::Log(FLOG_INFO, "NetIO::ThreadProcess(Server) - Connection failed");
            }

            inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
                FLog::Log(FLOG_INFO, "NetIO::ThreadProcess(Server) - Got connection from %s", s);

            //save client
            client.Create(new_fd, &messageQueue, SERVER);

            while(!m_bStop && m_running && client.isConnected()) {  // main recv() loop
                client.RecieveMessage();
            }
        }
        Close();
    } else if (role == CLIENT) {
        FLog::Log(FLOG_INFO, "NetIO::ThreadProcess(Client) - Waiting for messages");
        while(!m_bStop && m_running && client.isConnected()) {  // main recv() loop
            client.RecieveMessage();
        }
        Close();
    }
}
