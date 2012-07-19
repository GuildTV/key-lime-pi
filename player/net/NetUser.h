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

#ifndef NETUSER_H
#define NETUSER_H

#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "NetOpcode.h"
#include "NetMessageQueue.h"

#include "logger.h"

#define MAXDATASIZE 8448
#define DATAHEADER     2

#ifndef SHAKESERVER
#define SHAKESERVER "I am ALPHA-PI!! :)"
#endif
#ifndef SHAKECLIENT
#define SHAKECLIENT "I am RGB-PI!! :)"
#endif


using namespace std;

class NetUser {
public:
    NetUser();
    ~NetUser();

    void Create(int sockfd, NetMessageQueue* messageQueue, NetRole myrole);
    void Close();

    void RecieveMessage();
    bool SendMessage(string msg);

    int getFD(){return myfd;};
    bool isConnected(){return connected;};
protected:

private:
    int myfd;
    NetMessage partialMessage;
    NetMessageQueue* messageQueue;
    NetRole role;
    bool connected;
    bool handshaken;

    void processMessage(string str);
    bool processHandshake(string str);
    void CheckMessageFinished();
    bool SendMessageRAW(string msg);
};



#endif // NETUSER_H
