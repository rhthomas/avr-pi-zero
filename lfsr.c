// file    : lfsr.c
// author  : Rhys Thomas
// created : 2016-12-09
/* linear feedback shift register with taps at
 * bits 0 and 3. uses interrupts to trigger the
 * do{}while. triggering the do while inside the
 * interrupt meant that if the switch bounced it
 * would run again. this flag method only runs once!
 */

#define F_CPU 1000000L // 1MHz internal clock
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "regs.h"

// apparently bool is not a variable type in c?
typedef enum {false, true} bool;
volatile bool go;

void intInit(void)
{
    cli();
    MCUCR |= _BV(ISC01) | _BV(ISC00);
    GIMSK |= _BV(INT0);
    sei();
}

ISR(INT0_vect)
{
    go = true;
}

int main(void)
{
    shiftInit();
    intInit();
    shiftOut(0x00);

    uint8_t bit;
    uint8_t start_state = 0x2b;
    uint8_t lfsr = start_state;

    shiftOut(lfsr);

    for(;;) {
        if(go) {
            /* need to use do{}while rather than just while because at
             * the start lfsr=start_state and so the code would never
             * enter the while loop.
             */
            do {
                bit = ((lfsr & 0b00001000) ^ (lfsr & 0b00000001)) & 1;
                lfsr = (lfsr>>1) | (bit<<7);
                shiftOut(lfsr);
                _delay_ms(200);
            } while(lfsr!=start_state);
            // reset flag
            go = false;
        }
    }
    return 0;
}
