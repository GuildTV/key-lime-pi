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

/**
 * Create a message handler thread for messages from slave-pi
**/

#ifndef LIMEMASTERDOWNSTREAM_H
#define LIMEMASTERDOWNSTREAM_H

#include <pthread.h>
#include "logger.h"
#include "net/NetIO.h"

class LimeMaster;

class LimeMasterDownStream {
    protected:
        pthread_attr_t      m_tattr;
        struct sched_param  m_sched_param;
        pthread_mutex_t     m_lock;
        pthread_t           m_thread;
        volatile bool       m_running;

        LimeMaster *master;
        NetIO *netIO;

    private:
        static void *Run(void *arg);
        void Process();

    public:
        LimeMasterDownStream(LimeMaster *m, NetIO *net);
        ~LimeMasterDownStream();

        bool Running(){return m_running;}
        pthread_t ThreadHandle(){return m_thread;}

        bool Start();
        bool Stop();
};
#endif // LIMEMASTERDOWNSTREAM_H
