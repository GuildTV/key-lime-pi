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

#include "LimeTestPlay.h"

LimeTestPlay::LimeTestPlay() {
    renderer = new MyRender(NULL);
}

void LimeTestPlay::Run() {
    Json::Value k("yay!");
    Json::Value t;
    t["test"] = k;

    renderer->Create("data/new/script.json", t);
    renderer->Play();
    while(renderer->Running()){}
}

int main(int argc, char *argv[]){
    //open log
    FLog::Open("TestPlay.log");

    FLog::Log(FLOG_INFO, "Starting key-lime-pi test-play");
    printf("Starting key-lime-pi test-play\n");

    //create and run program
    LimeTestPlay lime;
    lime.Run();

    printf("Closing key-lime-pi test-play\n");
    FLog::Log(FLOG_INFO, "Closing key-lime-pi test-play\n\n");

    //close log
    FLog::Close();
    return 0;
}
