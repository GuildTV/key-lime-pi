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
        NetMessageQueue();
        ~NetMessageQueue();

        NetMessage* Pop(bool wait);
        int Size();
        bool isEmpty();
        void Push(NetMessage* msg);

    protected:
        void Lock();
        void Unlock();
        pthread_mutex_t m_lock;
        pthread_cond_t m_cond;

    private:
        queue<NetMessage *> messageQueue;
};

#endif // NETMESSAGEQUEUE_H
