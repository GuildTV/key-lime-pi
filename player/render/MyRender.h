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

#ifndef RENDERTEST
#include "OMX/OMXClock.h"
#include "OMX/OMXThread.h"
#include "OMX/OMXPlayerVideo.h"
#endif

#include "OverlayRenderer.h"

#include <deque>
#include <sys/types.h>

#include <string>

using namespace std;

/**
 * Spawn a thread from OMXLink to run the OpenGL renderer
**/
class MyRender
{
protected:
  bool                      m_open;
#ifndef RENDERTEST
  OMXClock                  *m_av_clock;
#endif

  pthread_attr_t      m_tattr;
  struct sched_param  m_sched_param;
  pthread_mutex_t     m_lock;
  pthread_t           m_thread;
  volatile bool       m_running;
  volatile bool       m_bStop;

  void Lock();
  void UnLock();
private:
  static void *Run(void *arg);
#ifndef RENDERTEST
  OMXPlayerVideo *vid;
#endif
  OverlayRenderer *renderer;
  NetIO *netIO;

public:
  MyRender(NetIO *net);
  ~MyRender();
#ifndef RENDERTEST
  bool Play(OMXClock *av_clock, OMXPlayerVideo *m_player_video);
#else
  bool Play();
#endif
  void Process();

  void Create(std::string file, Json::Value data);
  bool Running();
  pthread_t ThreadHandle();
  bool Close();
};

