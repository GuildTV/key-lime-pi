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

#include "OMXLink.h"

using namespace std;

class OMXWrapper
{
protected:
  bool                      m_open;

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
  OMXLink* omx;
  void Process();

public:
  OMXWrapper();
  ~OMXWrapper();
  bool Play();
  void Load(string f);

  bool Running();
  pthread_t ThreadHandle();
  bool Stop();
};
