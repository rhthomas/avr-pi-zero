// file    : uart.c
// author  : Rhys Thomas
// created : 2016-12-13
/* testing serial comms from the attiny using the debug.h
 * header. Tx of tiny is connected to Rx of Pi-Zero (GPIO13)
 * to see data run "screen /dev/ttyAMA0 9600"
 */

#define F_CPU 1000000L // 1MHz internal clock
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "lib/debug.h"

int main(void)
{
    setup_serial(PB3);
    sei(); // enable interrupts

    uint8_t count=0;
    char str[80]; // store sprintf string here

    // newline after the mess of flashing chip
    serial_print("\r\n");

    for(;;) {
        _delay_ms(75);
        // screen wants \r\n
        sprintf(str,"count: %i\r\n",count);
        serial_print(str);
        if(count==200) {
            sprintf(str,"overflow soon!\r\n");
            serial_print(str);
        }
        count++;
    }
    return 0;
}
