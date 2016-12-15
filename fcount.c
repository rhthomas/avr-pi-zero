// file    : fcount.c
// author  : Rhys Thomas
// created : 2016-12-14
/* frequency counter. see lab T3 from last year for comparator.
 * first get it working with digital signal from second tiny. then comparator
 * turns on when V>setpoint. the input pins ie. what triggers the external
 * interrupt, is min 0.6Vcc (1.98V) and max Vcc+0.5 (3.7V). so we'll have
 * to use mosfets to drive signals outside of the range 2<in<3.7
 */

#define F_CPU 1000000L // 1MHz internal clock
#include <avr/io.h>
#include <util/delay.h>
#include "debug.h"
#include <avr/interrupt.h>

// probably more space efficient than #include <bool.h>
typedef enum {false, true} bool;

bool first = false; // flag whether rising edge of input
uint16_t overflow=0; // number times the system overflows
float new_freq=0; // calculated frequency from ext. interrupt

void extInterrupt(void)
{
    // trigger interrupt on rising edge
    MCUCR |= _BV(ISC01) | _BV(ISC00);
    // set external interrupt
    GIMSK |= _BV(INT0);
}

// external interrupt. when input goes high flip the HIGH flag.
// if first  {start counter}
// if ~first {stop counter can calculate frequency from TCNT1 and overflow}
ISR(INT0_vect)
{
    first = ~first;
    // led to indicate interrupt has triggered
    PORTB ^= _BV(PB4);
    sendStrBySerial("interrupt\r\n");

    if(first) {
        // enable overflow interrupt
        TIMSK |= _BV(TOIE1);
        // reset counter just in case
        TCNT1 = 0;
    }
    else {
        // disable overflow interrupt
        TIMSK &= ~_BV(TOIE1);
        // calculate measured frequency
        uint16_t total_ticks = (overflow*255)+TCNT1;
        new_freq = F_CPU/total_ticks;
    }
}

void timer1_init(void)
{
    // no prescaling
    TCCR1 |= _BV(CS10);
    // initialise counter
    TCNT1 = 0;
}

// interrupt to count the number of overflows
ISR(TIMER1_OVF)
{
    overflow++;
}

int main(void)
{
    DDRB |= _BV(PB4);
    PORTB &= ~_BV(PB4);

    // initialise timer
    timer1_init();
    // initialise external interrupts
    extInterrupt();
    initSerial(PB3);
    sei();

    char str[80];

    for(;;){
        sprintf(str,"freq: %f\r\n",new_freq);
        sendStrBySerial(str);
        _delay_ms(250);
    }

    return 0;
}
