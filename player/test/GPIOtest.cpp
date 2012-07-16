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

#include "GPIO.h"
#include "logger.h"

#include <pthread.h>

int main(int argc, char *argv[]){
    FLog::Open("GPIOtest.log");

    GPIO gpioHandle;

    printf("Start of GPIO test\n");


    gpioHandle.BindInput(22, 100000);
    gpioHandle.BindOutput(21);
    if(!gpioHandle.WriteOutput(GPIO_LOW))
        printf("FAILED!!");

    gpioHandle.ThreadCreate();

    while(true){
        if(gpioHandle.CondWait(GPIO_HIGH, false, 1, 100)) {
            printf("got change\n");
            gpioHandle.WriteOutput(GPIO_HIGH);
            break;
        }

        printf("repeat again\n");
    }

    gpioHandle.ThreadStop();

    printf("End of GPIO test\n");

    FLog::Close();
}
