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
 * Test system clock accuracy.
 * Halfway through every second, output to GPIO21 (GPIO2/pin 13) for a short burst.
 * Designed to be run on two pi's and output shown on an oscilloscope, to measure difference
**/

#include <time.h>
#include <stdio.h>
#include <bcm2835.h>

int main (int argc, char *argv[]){
    //load GPIO and set the pin to output
    bcm2835_init();
    bcm2835_gpio_fsel(21, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_set_pud(21, BCM2835_GPIO_PUD_UP);

    //write low to GPIO
    bcm2835_gpio_write(21, LOW);

    //create some tracking value
    timespec ts;
    bool ready = true;

    //repeat this forever
    while(true){
        //get the current time from the system clock
        clock_gettime(CLOCK_REALTIME, &ts);

        //if we are in the first half of the second, set ready to true
        if(ts.tv_nsec < 500000000)
            ready = true;
        //first time we hit second half of the second
        else if(ready == true) {
            ready = false;
            //write HIGH to GPIO
            bcm2835_gpio_write(21, HIGH);
            //wait for a moment
            delay(5);
            //write LOW to GPIO
            bcm2835_gpio_write(21, LOW);
        }
    }

    return 0;
}
