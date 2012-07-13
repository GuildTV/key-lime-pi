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

#ifndef LOGGER_H
#define LOGGER_H

enum FLogLevels {FLOG_DEBUG, FLOG_INFO, FLOG_WARNING, FLOG_ERROR, FLOG_FATAL};

class FLog
{
    public:
        static void Open();
        static void Close();
        static void Log(FLogLevels logLevel, const char *format, ... );
    protected:
    private:
};

#endif // LOGGER_H
