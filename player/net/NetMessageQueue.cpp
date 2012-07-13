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
