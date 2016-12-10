// file    : regs.h
// author  : Rhys Thomas
// created : 2016-12-06
/* This header file contains the code
 * necessary in order to drive a 595
 * 8-bit shift register on an attiny85.
 */

#include <avr/io.h>

// pin 13 to GND, pin 10 to VCC
#define DATA  (1<<PB0) // pin 14
#define LATCH (1<<PB1) // pin 12
#define CLOCK (1<<PB3) // pin 11

void shiftInit(void)
{
    DDRB |= (DATA | LATCH | CLOCK); // set control pins as output
    PORTB &= ~(DATA | LATCH | CLOCK); // init low
}

void shiftOut(uint8_t val)
{
    for(uint8_t i=0; i<8; i++) {
        // tests MSB of val
        if(val & 0b10000000) {
            PORTB |= DATA;
        }
        else {
            PORTB &= ~DATA;
        }
        // pulse the clock
        PORTB |= CLOCK;
        PORTB &= ~CLOCK;
        val<<=1; // move next bit to MSB
    }
    // data set, move to output
    PORTB |= LATCH;
    PORTB &= ~LATCH;
}
