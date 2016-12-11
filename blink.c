// file    : blink.c
// author  : Rhys Thomas
// created : 2016-12-04
/* everyone's first sketch!
 */

#define F_CPU 1000000L // 1MHz internal clock
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    DDRB  = 0xFF; // PORTB is output, all pins
    PORTB = 0x00; // initialise pins to low

    for(;;) {
        PORTB ^= 0xFF; // invert all pins
        _delay_ms(1000);
    }
    return 0;
}

