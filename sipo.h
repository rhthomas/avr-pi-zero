// file    : sipo.h
// author  : Rhys Thomas
// created : 2016-12-06
/* This header file contains the code necessary in order
 * to drive a 595 8-bit shift register on an attiny85.
 * Connect pins 10 and 13 to VCC and GND respectivly
 */

#include <avr/io.h>

volatile struct {
    uint8_t data, latch, clock;
} regs = {0,0,0};

void shiftInit(uint8_t dataPin, uint8_t latchPin, uint8_t clockPin)
{
    regs.data = _BV(dataPin);
    regs.latch = _BV(latchPin);
    regs.clock = _BV(clockPin);

    // set control pins as outputs and initialise low
    DDRB |= (regs.data | regs.latch | regs.clock);
    PORTB &= ~(regs.data | regs.latch | regs.clock);
}

void shiftOut(uint8_t val)
{
    for(uint8_t i=0; i<8; i++) {
        // tests MSB of val
        if(val & 0b10000000) {
            PORTB |= regs.data;
        }
        else {
            PORTB &= ~regs.data;
        }
        // pulse the clock
        PORTB |= regs.clock;
        PORTB &= ~regs.clock;
        val<<=1; // move next bit to MSB
    }
    // data set, move to output
    PORTB |= regs.latch;
    PORTB &= ~regs.latch;
}
