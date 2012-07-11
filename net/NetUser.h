#ifndef NETUSER_H
#define NETUSER_H

#include <arpa/inet.h>
#include <string>
#include <string.h>

#include "NetOpcode.h"
#include "NetMessageQueue.h"

#define MAXDATASIZE 8448
#define DATAHEADER     2

using namespace std;

class NetUser {
public:
    NetUser();
    ~NetUser();

    void Create(int sockfd, NetMessageQueue* messageQueue);
    void Close();

    string RecieveMessage();
    bool SendMessage(string msg);

    int getFD(){return myfd;};
    bool isConnected(){return connected;};
protected:

private:
    int myfd;
    NetMessage partialMessage;
    NetMessageQueue* messageQueue;
    bool connected;

    string processMessage(string str);
    void CheckMessageFinished();
};



#endif // NETUSER_H
