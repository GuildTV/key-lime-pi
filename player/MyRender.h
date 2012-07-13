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

#include "OMX/OMXClock.h"
#include "OMX/OMXThread.h"

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

