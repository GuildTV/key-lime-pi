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

#include "logger.h"

#include <stdio.h>
#include <string>

#include "utils/StdString.h"

using namespace std;

static FILE* flogFile = NULL;

static char levelNames[][8] = {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};

void FLog::Open() {
    if(!flogFile){
        flogFile = fopen("GTVPlayer.log","a");
        if(!flogFile)
            printf("Failed to open log flogFile");
    }
}

void FLog::Close() {
    if(flogFile){
        fclose(flogFile);
        flogFile = NULL;
    }
}

void FLog::Log(FLogLevels logLevel, const char *format, ... ) {
    if(!flogFile)
        return;

    CStdString strData;

    va_list va;
    va_start(va, format);
    strData.FormatV(format,va);
    va_end(va);

    #ifdef DEBUGLOG
    if(logLevel == FLOG_DEBUG)
        return;
    #endif

    string levelName = levelNames[logLevel];

    fputs(levelName.c_str(), flogFile);
    fputs(": ", flogFile);
    fputs(strData, flogFile);
    fputs("\n", flogFile);

    fflush(flogFile);
}