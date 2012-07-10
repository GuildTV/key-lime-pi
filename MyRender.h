#include "OMXClock.h"
#include "OMXThread.h"

#include "OverlayRenderer.h"

#include <deque>
#include <sys/types.h>

#include <string>

using namespace std;

class MyRender : public OMXThread
{
protected:
  bool                      m_open;
  OMXClock                  *m_av_clock;
  bool                      m_bAbort;
  bool                      m_use_thread;

  void Lock();
  void UnLock();
private:
public:
  MyRender();
  ~MyRender();
  bool Open(OMXClock *av_clock, bool use_thread);
  bool Close();
  void Process();
};

