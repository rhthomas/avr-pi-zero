// file    : fcount.c
// author  : Rhys Thomas
// created : 2016-12-14
/* frequency counter. run with -u vfprintf -lprintf_flt
 * todo:
 * [x] get it working with 1Hz square wave
 *     ^ sort of, anything >20Hz works.
 *     ^ anything less comes out as 20Hz, this must be times to calculate freq etc?
 * [ ] use comparator on ADC
 */

#define F_CPU 1000000L // 1MHz internal clock
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "lib/debug.h"

volatile bool print=false; // flag to tell main to print
volatile uint16_t overflow=0; // number times the system overflows
volatile float freq;
volatile uint8_t count;

void timer1_init(void)
{
    TCCR1 |= _BV(CS10); // no prescaling
    TIMSK |= _BV(TOIE1); // enable overflow interrupt
}

void external_init(void)
{
    MCUCR |= _BV(ISC01) | _BV(ISC00); // trigger interrupt on rising edge
    GIMSK |= _BV(INT0); // set external interrupt
}

ISR(INT0_vect)
{
    count++;

    // if first rising edge, start counters
    if(count == 1) {
        overflow = 0;
        TCNT1 = 0;
        PORTB |= _BV(PB4);
    }
    // else, calculate freq and reset count
    else {
        freq = 1000000.0/(TCNT1+(overflow*255));
        count = 0; // reset count
        print = true;
        PORTB &= ~_BV(PB4);
    }
}

// count number of overflows
// this effects comms so ISR_NOBLOCK needed
ISR(TIMER1_OVF_vect,ISR_NOBLOCK){overflow++;}

int main(void)
{
    // debug leds
    DDRB |= _BV(PB4);
    PORTB &= ~_BV(PB4);

    initSerial(PB3);
    timer1_init();
    external_init();
    sei();

    char str[10];

    for(;;){
        if(print) {
            sprintf(str,"%.2fHz\r\n",freq);
            sendStrBySerial(str);
            print = false; // reset print flag
        }
    }
    return 0;
}
