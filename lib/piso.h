// file    : piso.h
// author  : Rhys Thomas
// created : 2017-01-04
/* Header contains functins to drive 74HC165 8-bit
 * parallel-in/serial-out shift register.
 * NOT YET TESTED!
 */

#include <avr/io.h>

volatile struct {
    uint8_t data, pload, clock, bits;
} piso;

void setup_piso(uint8_t dataPin, uint8_t ploadPin, uint8_t clockPin, uint8_t num)
{
    piso.data = _BV(dataPin);
    piso.pload = _BV(ploadPin);
    piso.clock = _BV(clockPin);
    piso.bits = 8*num;

    // piso inputs, initialise pload high and clock low
    DDRB |= (piso.pload | piso.clock);
    PORTB |= piso.pload; PORTB &= ~piso.clock;
    // piso outputs, enable pullups
    DDRB &= ~piso.data;
    PORTB |= piso.data;
}

int shift_in(void)
{
    int pisoVal=0, bitVal=0;

    // load parallel values to '165
    PORTB &= ~piso.pload;
    PORTB |= piso.pload;

    // get bits stored in '165
    for(int i=0; i<piso.bits; i++) {
        bitVal = PINB & piso.data; // read value of data pin
        pisoVal |= (bitVal << (piso.bits-1-i));
        // cycle clock for next value
        PORTB |= piso.clock;
        PORTB &= ~piso.clock;
    }
    return pisoVal;
}
