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

#include "NetUser.h"

NetUser::NetUser(){
    //define as unconnected
    connected = false;
    handshaken = false;
    role = UNDEFINED;

    //define partialMessage as empty
    partialMessage.length = 0;
    partialMessage.complete = true;
    partialMessage.user = this;
    partialMessage.message = "";
}
NetUser::~NetUser(){
    //close connection
    Close();
}

void NetUser::RecieveMessage() {
    if(!connected)
        return;

    //recieve message
    char buf[MAXDATASIZE+DATAHEADER];
    memset(&buf, 0, MAXDATASIZE+DATAHEADER);
    int numbytes = recv(myfd, &buf, MAXDATASIZE+DATAHEADER-1, 0);

    //close connection on empty message
    if(numbytes == 0)
        Close();

    //discard message if too short for header
    if(numbytes <= 2)
        return;

    //send message to appropriate function based upon connection status
    if(handshaken)
        processMessage(buf);
    else
        processHandshake(buf);
}

void NetUser::processMessage(string str){
    if(!connected)
        return;

    //extract the opcode from the header
    int opcode = (str[0] - '0');
    opcode = opcode&7;

    //extract the message length from the header
    int len1 = (str[0] - '0') + 48;
    int len2 = (str[1] - '0') + 48;
    len1 = len1&248;
    int len = len2 + 32*len1;

    //trim header off message  body
    string msg = str.substr(2);

    //cast the opcode
    NetOpcode op = (NetOpcode)opcode;
    switch(op){
    case OP_COMMAND:
        //discard partialMessage if it was incomplete
        if(!partialMessage.complete)
            FLog::Log(FLOG_DEBUG, "NetUser::processMessage - Discarded incomplete message");

        //overwrite partialMessage with this new one
        partialMessage.length = len;
        partialMessage.complete = false;
        partialMessage.user = this;
        partialMessage.message = msg;

        //check if we have recieved the whole message
        CheckMessageFinished();
    break;
    case OP_COMMAND_CONT:
        //discard fragment if partialMessage is complete
        if(partialMessage.complete){
            FLog::Log(FLOG_DEBUG, "NetUser::processMessage - Discarded stray message fragment");
        } else {
             partialMessage.message.append(msg);

            CheckMessageFinished();
        }
    break;
    default:
        //log unknown message
        FLog::Log(FLOG_DEBUG, "NetUser::processMessage - Received message with unknown opcode %d", opcode);
    break;
    }
}

bool NetUser::processHandshake(string str){
    if(role == SERVER){
        if(str.compare(SHAKECLIENT) == 0){
            handshaken = true;
            FLog::Log(FLOG_DEBUG, "NetUser::processHandshake(Server) - Responding to handshake");
            SendMessageRAW(SHAKESERVER);
            return true;
        } else {
            FLog::Log(FLOG_ERROR, "NetUser::processHandshake(Server) - Received invalid handshake");
            Close();
            return false;
        }
    } else if (role == CLIENT) {
        if(str.compare(SHAKESERVER) == 0){
            handshaken = true;
            FLog::Log(FLOG_DEBUG, "NetUser::processHandshake(Client) - Handshake Successful");
            return true;
        } else {
            FLog::Log(FLOG_ERROR, "NetUser::processHandshake(Client) - Received invalid handshake");
            Close();
            return false;
        }
    }

    return false;
}

void NetUser::CheckMessageFinished() {
    //get actual and desired message lengths
    int desiredLength = partialMessage.length;
    int actualLength = partialMessage.message.length();

    //actual length is too short. stop.
    if(actualLength < desiredLength)
        return;
    //actual length is correct. message is complete.
    else if (actualLength == desiredLength) {
        partialMessage.complete = true;
        (*messageQueue).Push(&partialMessage);
    //actual length is too long.
    } else if (actualLength > desiredLength) {
        //split message
        string complete = partialMessage.message.substr(0, partialMessage.length);
        string next = partialMessage.message.substr(partialMessage.length);
        //save completed message
        partialMessage.message = complete;
        partialMessage.complete = true;
        (*messageQueue).Push(&partialMessage);

        //process extra message
        processMessage(next);
    }
}

bool NetUser::SendMessageRAW(string msg){
    if(!connected)
        return false;

    while(msg.length() > 0 && connected){
        int sent = send(myfd, (char*)msg.c_str(), msg.length(), 0);
        msg = msg.substr(sent);

        //TODO timeout??
    }

    return true;
}

bool NetUser::SendMessage(string msg){
    if(!connected)
        return false;

    //fail if message is too long
    if(msg.length() > MAXDATASIZE)
        return false;

    //set as first fragment
    bool first = true;

    while(msg.length() > 0 && connected){
        //determine length bytes
        int length = msg.length();
        int len1 = length/256;
        len1 *= 8;
        int len2 = length-len1*32;

        //determine opcode
        int opcode;
        if(first)
            opcode = (int)OP_COMMAND;
        else
            opcode = (int)OP_COMMAND_CONT;

        int bit1 = len1+opcode;

        //compole header
        char b1 = char(bit1);
        char b2 = char(len2);

        //add header
        msg = b2 + msg;
        msg = b1 + msg;

        int sent = send(myfd, (char*)msg.c_str(), msg.length(), 0);

        int remaining = msg.length()-sent;

        //if only header/partial header was sent, then trim it off before continuing
        if(remaining >= length) {
            msg = msg.substr(remaining-length);
            continue;
        }

        msg = msg.substr(sent);

        first = false;

        //TODO - add timeout
    }
    return true;
}

void NetUser::Create(int sockfd, NetMessageQueue* que, NetRole myrole) {
    if(connected)
        return;

    //save basic data
    myfd = sockfd;
    messageQueue = que;
    connected = true;
    role = myrole;

    //start handshake if i am a client
    if(role == CLIENT){
        //initiate handshake
        FLog::Log(FLOG_INFO, "NetUser::Create(Client) - Starting handshake");
        SendMessageRAW(SHAKECLIENT);
    }
}

void NetUser::Close() {
    if(!connected)
        return;
    //send null byte
    send(myfd, '\0', 0, 0);
    FLog::Log(FLOG_INFO, "NetUser::Close - Closed connection");
    //tidy up
    connected = false;
    handshaken = false;
    role = UNDEFINED;
    //close connection
    close(myfd);
}
