#include "MyRender.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

MyRender::MyRender()
{

  pthread_mutex_init(&m_lock, NULL);
}

MyRender::~MyRender()
{
  Close();

  pthread_mutex_destroy(&m_lock);
}

void MyRender::Lock()
{
  if(m_use_thread)
    pthread_mutex_lock(&m_lock);
}

void MyRender::UnLock()
{
  if(m_use_thread)
    pthread_mutex_unlock(&m_lock);
}


bool MyRender::Open(OMXClock *av_clock, bool use_thread)
{
  if (!av_clock)
    return false;
  
  if(ThreadHandle())
    Close();
	
  m_av_clock    = av_clock;
  m_use_thread  = use_thread;

  //m_FlipTimeStamp = m_av_clock->GetAbsoluteClock();

  if(m_use_thread)
    Create();

  return true;
}

bool MyRender::Close()
{
  m_bAbort  = true;

  if(ThreadHandle())
  {
    StopThread();
  }

  return true;
}


void MyRender::Process() {
	OverlayRenderer renderer(1);
	
	double lastTime = 80000.0;
  while(!m_bStop && !m_bAbort){
	  double currentTime = m_av_clock->GetPTS();
	  if(currentTime != lastTime){
		  lastTime = currentTime;
		  //update here
		  
	  } 
		//render here??
		renderer.show_next();
	  
  }
}
