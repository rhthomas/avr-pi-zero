// file    : sipo.h
// author  : Rhys Thomas
// created : 2016-12-06
/* Header contains the functions to drive 74HC595 8-bit
 * serial-in/parallel-out shift regsiter.
 */

#include <avr/io.h>
#include <math.h>

volatile struct {
    uint8_t data, latch, clock, bits;
} sipo;

void setup_sipo(uint8_t dataPin, uint8_t latchPin, uint8_t clockPin, uint8_t num)
{
    sipo.data = _BV(dataPin);
    sipo.latch = _BV(latchPin);
    sipo.clock = _BV(clockPin);
    sipo.bits = 8*num;

    // set control pins as outputs and initialise low
    DDRB |= (sipo.data | sipo.latch | sipo.clock);
    PORTB &= ~(sipo.data | sipo.latch | sipo.clock);
}

void shift_out(uint8_t val)
{
    for(uint8_t i=0; i<sipo.bits; i++) {
        // tests MSB of val
        if(val & (int)pow(2,sipo.bits-1)) { // 0b10000000 for 1 '595
            PORTB |= sipo.data;
        }
        else {
            PORTB &= ~sipo.data;
        }
        // pulse the clock
        PORTB |= sipo.clock;
        PORTB &= ~sipo.clock;
        val<<=1; // move next bit to MSB
    }
    // data set, move to output
    PORTB |= sipo.latch;
    PORTB &= ~sipo.latch;
}
