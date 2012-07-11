#include "NetMessageQueue.h"

NetMessage* NetMessageQueue::Pop() {
    Lock();

    NetMessage* msg = messageQueue.front();
    messageQueue.pop();

    Unlock();

    return msg;
}

int NetMessageQueue::Size() {
    Lock();

    int size = messageQueue.size();

    Unlock();

    return size;
}

bool NetMessageQueue::isEmpty() {
    Lock();

    bool empty = messageQueue.empty();

    Unlock();

    return empty;
}

void NetMessageQueue::Push(NetMessage* msg) {
    Lock();

    messageQueue.push(msg);

    Unlock();
}

void NetMessageQueue::Lock() {
    pthread_mutex_lock(&m_lock);
}
void NetMessageQueue::Unlock() {
    pthread_mutex_unlock(&m_lock);
}
