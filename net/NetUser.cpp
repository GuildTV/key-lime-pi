#include "NetUser.h"

NetUser::NetUser(){
    connected = false;
    handshaken = false;
    role = UNDEFINED;

    partialMessage.length = 0;
    partialMessage.complete = true;
    partialMessage.user = this;
    partialMessage.message = "";
}
NetUser::~NetUser(){
    Close();
}

string NetUser::RecieveMessage() {
    if(!connected)
        return "";

    char buf[MAXDATASIZE+DATAHEADER];
    int numbytes = recv(myfd, buf, MAXDATASIZE+DATAHEADER-1, 0);

    if(numbytes == 0)
        close(myfd); //TODO

    if(numbytes <= 4)
        return "";  //TODO error

    if(handshaken)
        return processMessage(buf);

    processHandshake(buf);
    return "";
}

string NetUser::processMessage(string str){
    if(!connected)
        return "";

    int opcode = (str[0] - '0');
    opcode = opcode&7;

    int len1 = (str[0] - '0') + 48;
    int len2 = (str[1] - '0') + 48;
    len1 = len1&248;
    int len = len2 + 32*len1;

    string msg = str.substr(2, str.length()-2);//last digit is 4 when telnet, 2 when c++ :P

    NetOpcode op = (NetOpcode)opcode;
    switch(op){
    case OP_COMMAND:
        if(!partialMessage.complete)
            printf("Discarded incomplete message\n");

        partialMessage.length = len;
        partialMessage.complete = false;
        partialMessage.user = this;
        partialMessage.message = msg;

        CheckMessageFinished();
    break;
    case OP_COMMAND_CONT:
        if(partialMessage.complete){
            printf("Discarded lost message piece\n");
        } else {
             partialMessage.message.append(msg);

            CheckMessageFinished();
        }
    break;
    default:
        printf("Received message with unknown opcode %d\n", opcode);
    break;
    }

    return msg;
}

bool NetUser::processHandshake(string str){
    //str = str.substr(0, str.length()-2);

    if(role == SERVER){
        if(str.compare(SHAKECLIENT) == 0){
            handshaken = true;
            printf("Sending handshake to client\n");
            SendMessageRAW(SHAKESERVER);
            return true;
        } else {
            Close();
            printf("%s\n", str.c_str());
            return false;
        }
    } else if (role == CLIENT) {
        if(str.compare(SHAKESERVER) == 0){
            handshaken = true;
            printf("Handshake with server completed\n");
            return true;
        } else {
            Close();
            return false;
        }
    }

    return false;
}

void NetUser::CheckMessageFinished() {
    int desiredLength = partialMessage.length;
    int actualLength = partialMessage.message.length();

    if(actualLength < desiredLength)
        return;
    else if (actualLength == desiredLength) {
        partialMessage.complete = true;
        (*messageQueue).Push(&partialMessage);
    } else if (actualLength > desiredLength) {
        string complete = partialMessage.message.substr(0, partialMessage.length);
        string next = partialMessage.message.substr(partialMessage.length);
        partialMessage.message = complete;
        partialMessage.complete = true;
        (*messageQueue).Push(&partialMessage);

        processMessage(next);
    }
}

bool NetUser::SendMessageRAW(string msg){
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

    if(msg.length() > MAXDATASIZE)
        return false;

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
        unsigned char b1 = bit1;
        unsigned char b2 = len2;
        char header[2];
        header[0] = b1;
        header[1] = b2;

        //add header
        msg = header + msg;

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

    myfd = sockfd;
    messageQueue = que;
    connected = true;
    role = myrole;

    if(role == CLIENT){
        //initiate handshake
        printf("Starting handshake with server\n");
        SendMessageRAW(SHAKECLIENT);
    }
}

void NetUser::Close() {
    if(!connected)
        return;

    printf("Closed connection\n");
    connected = false;
    handshaken = false;
    role = UNDEFINED;
    close(myfd);
}
