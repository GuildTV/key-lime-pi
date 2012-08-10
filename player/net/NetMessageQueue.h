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

/**
 * Store a queue of recieved messages waiting to be processed
**/

#ifndef NETMESSAGEQUEUE_H
#define NETMESSAGEQUEUE_H

#include <string>
#include <queue>
using namespace std;

class NetUser;

//struct to store message data
typedef struct NetMessage {
    NetUser* user;
    int length;
    string message;
    bool complete;
} NetMessage;

class NetMessageQueue
{
    public:
        NetMessageQueue();
        ~NetMessageQueue();

        //get next message in queue
        NetMessage* Pop(bool wait);
        //get number of messages in queue
        int Size();
        //check if the queue is emtpy
        bool isEmpty();
        //push a message onto the end of the queue
        void Push(NetMessage* msg);
        //interupt any waiting threads
        void Interrupt();

    protected:
        void Lock();
        void Unlock();
        pthread_mutex_t m_lock;
        pthread_cond_t m_cond;

    private:
        //queue of messages
        queue<NetMessage *> messageQueue;
};

#endif // NETMESSAGEQUEUE_H
