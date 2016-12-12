// file    : coffee.c
// author  : Rhys Thomas
// created : 2016-12-07
/* 4 minute coffee timer for when brewing with
 * cafetiere.
 */

#define F_CPU 1000000L // 1MHz internal clock
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "sipo.h"

volatile uint8_t time;

void flash(void)
{
    for(uint8_t i=0; i<10; i++) {
        shiftOut(0xff);
        _delay_ms(50);
        shiftOut(0x00);
        _delay_ms(50);
    }
}

void intInit(void)
{
    cli();
    MCUCR |= _BV(ISC01) | _BV(ISC00);
    GIMSK |= _BV(INT0);
    sei();
}

ISR(INT0_vect)
{
    time = 0xff;
}

int main(void)
{
    intInit();
    shiftInit();

    // clear register of old code data
    shiftOut(0x00);

    for(;;) {
        if(time!=0) {
            shiftOut(time);
            // 4mins, 8 leds, 30s/led
            _delay_ms(30000);
            time>>=1;
            if(time==0) {
                flash();
            }
        }
    }
    return 0;
}
