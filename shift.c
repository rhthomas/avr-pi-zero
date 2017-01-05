// file    : shift.c
// author  : Rhys Thomas
// created : 2016-12-05
/* demo code for regs.h by counting from 0 to 255
 * and displaying the binary number on the leds.
 */

#define F_CPU 1000000L // 1MHz internal clock
#include <avr/io.h>
#include <util/delay.h>
#include "lib/sipo.h"

int main(void)
{
    // data,latch,clock,num
    setup_sipo(PB0,PB1,PB3,1);

    for(;;) {
        // counts from 0 to 255 and displays on leds
        for(uint8_t msg=0; msg<256; msg++) {
            shift_out(msg);
            _delay_ms(100);
        }
    }
    return 0;
}
