#ifndef RENDERTEST_H
#define RENDERTEST_H

#include "net/NetIO.h"
#include <string>
#include "logger.h"
#include "OverlayRenderer.cpp"

class RenderTest
{
    public:
        RenderTest();
        virtual ~RenderTest();
        void Run();
        void Stop(){run=false;};
    protected:
        void VideoLoad(std::string name);
        bool FileExists(const char * filename);
    private:
        NetIO net;
        bool run;
        void HandleMessage(NetMessage* msg);
};

#endif // RENDERTEST_H
