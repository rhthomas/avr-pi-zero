// file    : sipo.h
// author  : Rhys Thomas
// created : 2016-12-06
/* Header contains the functions to drive 74HC595 8-bit
 * serial-in/parallel-out shift regsiter.
 */

#include <avr/io.h>
#include <math.h>

volatile struct {
    uint8_t data, latch, clock, num;
} regs;

void shiftInit(uint8_t dataPin, uint8_t latchPin, uint8_t clockPin, uint8_t num)
{
    regs.data = _BV(dataPin);
    regs.latch = _BV(latchPin);
    regs.clock = _BV(clockPin);
    regs.num = num; // number of 595's connected in chain

    // set control pins as outputs and initialise low
    DDRB |= (regs.data | regs.latch | regs.clock);
    PORTB &= ~(regs.data | regs.latch | regs.clock);
}

void shiftOut(uint8_t val)
{
    for(uint8_t i=0; i<(8*regs.num); i++) {
        // tests MSB of val
        if(val & pow(2,(8*regs.num)-1)) { // 0b10000000 for one 595
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
