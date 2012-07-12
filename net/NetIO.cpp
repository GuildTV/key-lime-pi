/*
 * NetIO.cpp
 *
 *  Created on: 10 Jul 2012
 *      Author: julus
 */

#include "NetIO.h"
#include <netdb.h>

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
    printf("NetIO::%s - No thread running\n", __func__);
    return false;
  }

  m_bStop = true;
  pthread_join(m_thread, NULL);
  m_running = false;

  m_thread = 0;

  printf("NetIO::%s - Thread stopped\n", __func__);
  return true;
}

bool NetIO::ThreadCreate()
{
  if(m_running)
  {
    printf("NetIO::%s - Thread already running\n", __func__);
    return false;
  }

  m_bStop    = false;
  m_running = true;

  pthread_create(&m_thread, &m_tattr, &NetIO::ThreadRun, this);

  printf("NetIO::%s - Thread with id %d started\n", __func__, (int)m_thread);
  return true;
}

bool NetIO::ThreadRunning()
{
  return m_running;
}

pthread_t NetIO::ThreadHandle()
{
  return m_thread;
}

void *NetIO::ThreadRun(void *arg)
{
  NetIO *thread = static_cast<NetIO *>(arg);
  thread->ThreadProcess();

  printf("NetIO::%s - Exited thread with  id %d\n", __func__, (int)thread->ThreadHandle());
  pthread_exit(NULL);
}

void NetIO::Close() {
    printf("Socket closed\n");
    close(thisSocketFD);
    client.Close();
    role = UNDEFINED;
}

int NetIO::CreateServer(string port) {
    struct addrinfo hints, *servinfo, *p;
    struct sigaction sa;
    int yes=1;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, port.c_str(), &hints, &servinfo)) != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((thisSocketFD = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(thisSocketFD, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (setsockopt(thisSocketFD, IPPROTO_TCP, TCP_NODELAY, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(thisSocketFD, p->ai_addr, p->ai_addrlen) == -1) {
            close(thisSocketFD);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (listen(thisSocketFD, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    role = SERVER;
    ThreadCreate();

    return 0;
}

int NetIO::CreateClient(string address, string port) {
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(address.c_str(), port.c_str(), &hints, &servinfo)) != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((thisSocketFD = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(thisSocketFD, p->ai_addr, p->ai_addrlen) == -1) {
            close(thisSocketFD);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        printf("client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    client.Create(thisSocketFD, &messageQueue, CLIENT);

    role = CLIENT;
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
            printf("server: waiting for connections...\n");
            sin_size = sizeof their_addr;
            new_fd = accept(thisSocketFD, (struct sockaddr *)&their_addr, &sin_size);
            if (new_fd == -1) {
                   perror("accept");
            }

            inet_ntop(their_addr.ss_family,
                    get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
                printf("server: got connection from %s\n", s);

            //save client
            client.Create(new_fd, &messageQueue, SERVER);

            while(!m_bStop && m_running && client.isConnected()) {  // main recv() loop
                client.RecieveMessage();
            }
        }
        Close();
    } else if (role == CLIENT) {
        while(!m_bStop && m_running && client.isConnected()) {  // main recv() loop
            client.RecieveMessage();
        }
        Close();
    }
}
