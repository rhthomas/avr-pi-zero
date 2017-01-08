// file    : lfsr.c
// author  : Rhys Thomas
// created : 2016-12-09
/* linear feedback shift register with taps at
 * bits 0 and 3.
 */

#define F_CPU 1000000L // 1MHz internal clock
#include <avr/io.h>
#include <util/delay.h>
#include "lib/sipo.h"

int main(void)
{
    // data,latch,clock
    setup_sipo(PB0,PB1,PB3);
    shift_out(0x00);

    DDRB &= ~_BV(PB2); // set PB2 as input
    PORTB |= _BV(PB2); // enable pullups

    uint8_t bit;
    uint8_t start_state = 0x2b;
    uint8_t lfsr = start_state;

    // show the initial state
    shift_out(lfsr);

    for(;;) {
        if(PINB & _BV(PB2)) {
            /* need to use do{}while rather than just while because at
             * the start lfsr=start_state and so the code would never
             * enter the while loop.
             */
            do {
                bit = ((lfsr & 0b00001000) ^ (lfsr & 0b00000001)) & 1;
                lfsr = (lfsr>>1) | (bit<<7);
                shift_out(lfsr);
                _delay_ms(200);
            } while(lfsr!=start_state);
        }
    }
    return 0;
}
