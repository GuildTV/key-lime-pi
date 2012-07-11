#ifndef NETMESSAGEQUEUE_H
#define NETMESSAGEQUEUE_H

#include <string>
#include <queue>
using namespace std;

class NetUser;

typedef struct NetMessage {
    NetUser* user;
    int length;
    string message;
    bool complete;
} NetMessage;

class NetMessageQueue
{
    public:

        NetMessage* Pop();
        int Size();
        bool isEmpty();
        void Push(NetMessage* msg);

    protected:
        void Lock();
        void Unlock();
        pthread_mutex_t m_lock;

    private:
        queue<NetMessage *> messageQueue;
};

#endif // NETMESSAGEQUEUE_H
