#ifndef NETUSER_H
#define NETUSER_H

#include <arpa/inet.h>
#include <string.h>

#include "NetOpcode.h"
#include "NetMessageQueue.h"

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

    string RecieveMessage();
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

    string processMessage(string str);
    bool processHandshake(string str);
    void CheckMessageFinished();
    bool SendMessageRAW(string msg);
};



#endif // NETUSER_H
