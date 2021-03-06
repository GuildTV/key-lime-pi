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
 * Main program for key-lime-pi slave device
**/

#ifndef LIMESLAVE_H
#define LIMESLAVE_H

#include "LimeShared.h"

class LimeSlave: public LimeShared
{
    public:
        //run program
        void Run();
    protected:
        //determine play start time
        void playProcess(Json::Value *root, long *sec, long *nano);
        void dataListProcess(Json::Value *root);
    private:
};

#endif // LIMESLAVE_H
