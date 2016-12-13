// file    : test.c
// author  : Rhys Thomas
// created : 2016-12-13
/* testing the debug.h header
 */

#define F_CPU 1000000L // 1MHz internal clock
#include <avr/io.h>
#include <util/delay.h>
#include "debug.h"

int main(void)
{
    initSerial(PB0);
    sei();

    for(;;) {
        sendStrBySerial("Hello, world!\r\n");
        _delay_ms(1000);
    }

    return 0;
}
