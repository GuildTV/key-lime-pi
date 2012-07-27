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

#include "NetMessageQueue.h"

#include "logger.h"

NetMessageQueue::NetMessageQueue() {
    pthread_mutex_init(&m_lock, NULL);
    pthread_cond_init(&m_cond, NULL);
}

NetMessageQueue::~NetMessageQueue() {
    pthread_mutex_destroy(&m_lock);
    pthread_cond_destroy(&m_cond);
}

NetMessage* NetMessageQueue::Pop(bool wait) {
    //if there is a message waiting
    if(Size() > 0){
        Lock();
        //get the message, and remove it from the queue
        NetMessage* msg = messageQueue.front();
        messageQueue.pop();

        Unlock();
        return msg;
    }

    //if we have been told to wait
    if (wait) {
        Lock();

        //wait for message
        pthread_cond_wait(&m_cond, &m_lock);

        //get the message, and remove it from the queue
        NetMessage* msg = messageQueue.front();
        messageQueue.pop();

        Unlock();
        return msg;
    } else {
        //otherwise return NULL
        return NULL;
    }

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

    //add message to queue
    messageQueue.push(msg);

    //broadcast to any threads waiting in a pop
    pthread_cond_broadcast(&m_cond);

    Unlock();
}

void NetMessageQueue::Lock() {
    pthread_mutex_lock(&m_lock);
}
void NetMessageQueue::Unlock() {
    pthread_mutex_unlock(&m_lock);
}
