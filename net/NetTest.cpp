#include "NetIO.h"
#include "NetMessageQueue.h"

#include <string.h>
#include <stdio.h>
#include <iostream>
#include <pthread.h>

class NetTest{
public:
    NetTest(NetIO* net);
    bool ThreadCreate();
    pthread_t ThreadHandle();
    bool ThreadStop();
    bool ThreadRunning() {return running;};

protected:
    pthread_mutex_t     m_queue_lock;
    pthread_attr_t      m_tattr;
    struct sched_param  m_sched_param;
    pthread_t           m_thread;

private:
    static void *ThreadRun(void *arg);
    void ThreadProcess();
    NetIO* netio;
    bool running;
};

NetTest::NetTest(NetIO* net){
    netio = net;
}

bool NetTest::ThreadStop()
{
  running = false;

  pthread_join(m_thread, NULL);

  m_thread = 0;

  printf("NetTest::%s - Thread stopped\n", __func__);
  return true;
}

bool NetTest::ThreadCreate()
{
  pthread_create(&m_thread, &m_tattr, &NetTest::ThreadRun, this);
  running = true;

  printf("NetTest::%s - Thread with id %d started\n", __func__, (int)m_thread);
  return true;
}

pthread_t NetTest::ThreadHandle()
{
  return m_thread;
}

void *NetTest::ThreadRun(void *arg)
{
  NetTest *thread = static_cast<NetTest *>(arg);
  thread->ThreadProcess();

  printf("NetTest::%s - Exited thread with  id %d\n", __func__, (int)thread->ThreadHandle());
  pthread_exit(NULL);
}

void NetTest::ThreadProcess() {
    cout << "Message printer ready" << endl;
    NetMessageQueue* que = (*netio).GetMessageQueue();
    while(running){
        while(!(*que).isEmpty()) {
            NetMessage* mess = (*que).Pop();
            cout << "Recieved: " << (*mess).message << endl;
        }
    }

    running = false;
    cout << "Connection closed" << endl;
}

int main(int argc, char *argv[]){
    NetIO net;

    string type;
    cout << "Please specify client or server" << endl;
    cout << "> ";
    cin >> type;

    if(type.substr(0,1).compare("s") == 0){
        string port;
        cout << "Creating server" << endl;
        cout << "What port? (6789)" << endl;
        cout << "> ";
        cin >> port;
        if(port.compare(".") == 0)
            port = "6789";

        net.CreateServer(port);
    } else {
        string address, port;
        cout << "Creating client" << endl;
        cout << "Server address" << endl;
        cout << "> ";
        cin >> address;
        if(address.compare(".") == 0)
            address = "127.0.0.1";
        cout << "What port? (6789)" << endl;
        cout << "> ";
        cin >> port;
        if(port.compare(".") == 0)
            port = "6789";

        net.CreateClient(address, port);
    }

    cout << endl << endl;
    cout << "Creating message printer" << endl;

    NetTest test(&net);
    test.ThreadCreate();

    bool run = true;

    cout << "Starting input loop. type TERMINATE to exit gracefully" << endl;
    string in;
    while(run && test.ThreadRunning()){
        cout << "> ";
        cin >> in;
        if(in.compare("TERMINATE") == 0){
            run = false;
            break;
        }

        if(!(*net.GetClient()).isConnected())
            break;
        (*net.GetClient()).SendMessage(in);

    }


    test.ThreadStop();
    net.Close();


    return 0;
}
