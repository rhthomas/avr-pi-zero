// file    : fcount.c
// author  : Rhys Thomas
// created : 2016-12-14
/* frequency counter. run with -u vfprintf -lprintf_flt
 * todo:
 * [ ] get it working with 1Hz square wave
 * [ ] use comparator on ADC (see T3 lab)
 */

#define F_CPU 1000000L // 1MHz internal clock
#include <avr/io.h>
#include <avr/interrupt.h>
#include "debug.h"

typedef enum {false, true} bool;

volatile bool print=false; // flag to tell main to print
volatile uint16_t overflow=0; // number times the system overflows
volatile float new_freq=0.0; // calculated frequency from ext. interrupt
volatile uint8_t count=0;

void timer1_init(void)
{
    TCCR1 |= _BV(CS10); // no prescaling
    TIMSK |= _BV(TOIE1); // enable overflow interrupt
}

void extInterrupt(void)
{
    MCUCR |= _BV(ISC01) | _BV(ISC00); // trigger interrupt on rising edge
    GIMSK |= _BV(INT0); // set external interrupt
}

ISR(INT0_vect)
{
    count++;

    // if first rising edge, reset counters
    if(count == 1) {
        TCNT1 = 0;
        overflow = 0;
        PORTB |= _BV(PB4);
    }
    // else, calculate freq and reset count
    else {
        float total_ticks = TCNT1+(overflow*255);
        new_freq = 1000000.0/total_ticks;
        count = 0; // reset count
        print = true;
        PORTB &= ~_BV(PB4);
    }
}

// count number of overflows
// this interrupts comms so ISR_NOBLOCK needed
ISR(TIMER1_OVF_vect,ISR_NOBLOCK){overflow++;}

int main(void)
{
    DDRB |= _BV(PB4);
    PORTB &= ~_BV(PB4);

    initSerial(PB3);
    timer1_init();
    extInterrupt();
    sei();

    char str[80];

    for(;;){
        if(print) {
            sprintf(str,"freq: %.2f\r\n",new_freq);
            sendStrBySerial(str);
            print = false; // reset print flag
        }
    }
    return 0;
}
