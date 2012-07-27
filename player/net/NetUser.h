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

/**
 * Represents a connected user in a NetIO instance
**/
class NetUser {
public:
    NetUser();
    ~NetUser();

    //setup this NetUser
    void Create(int sockfd, NetMessageQueue* messageQueue, NetRole myrole);
    //close the connection
    void Close();

    //get the next message from the network stream
    void RecieveMessage();
    //send a message to the user
    bool SendMessage(string msg);

    //get the FD of the connection
    int getFD(){return myfd;};
    //returns true if connected
    bool isConnected(){return connected;};
protected:

private:
    //file descriptor
    int myfd;
    //partial recieved message
    NetMessage partialMessage;
    //recieved message queue
    NetMessageQueue* messageQueue;
    //role in the network model (client/server)
    NetRole role;
    //connected status
    bool connected;
    bool handshaken;

    //process recieved string
    void processMessage(string str);
    //process recieved handshake
    bool processHandshake(string str);
    //check if the partialMessage is actually complete
    void CheckMessageFinished();
    //send a raw messge without message wrappers
    bool SendMessageRAW(string msg);
};



#endif // NETUSER_H
