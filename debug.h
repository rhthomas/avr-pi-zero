// file    : debug.c
// author  : Rhys Thomas
// created : 2016-12-13
/* header for uart communications between attiny and GPIO15 of the
 * raspberry pi. adapted from http://www.idogendel.com/en/archives/406
 * to use the 1MHz internal clock of the tiny85
 */

#include <avr/io.h>
#include <avr/interrupt.h>

volatile struct {
    uint8_t dataByte, bitsLeft, pin, done;
} txData = {0,0,0,0};

void sendBySerial(const uint8_t data)
{
    txData.dataByte = data;
    txData.bitsLeft = 10;
    txData.done = 0;
    // reset counter
    TCNT0 = 0;
    // activate timer0 A-match interrupt
    TIMSK |= _BV(OCIE0A);
}

void sendStrBySerial(char *p)
{
    while(*p != '\0') {
        sendBySerial(*p++);
        while(!txData.done);
    }
}

void initSerial(const uint8_t portPin)
{
    txData.pin = portPin;
    // set pin as output
    DDRB |= _BV(txData.pin);
    PORTB |= _BV(txData.pin);
    // 1MHz/9600 = 104.166
    // (104/8)-1 = 12, clk prescaler of 8
    OCR0A = 12; // top value of counter0
    TCCR0A |= _BV(WGM01); // ctc mode
    TCCR0B |= _BV(CS01); // clk/8 prescaler
}

ISR(TIMER0_COMPA_vect)
{
    uint8_t bitval;

    switch(txData.bitsLeft) {
        case 10: bitval = 0; break; // start bit
        case  1: bitval = 1; break; // stop bit
        case  0: TIMSK &= ~_BV(OCIE0A);
                 txData.done = 1;
                 return;
        default: bitval = txData.dataByte & 1;
                 txData.dataByte >>= 1;
    }

    if(bitval) PORTB |= _BV(txData.pin);
    else PORTB &= ~_BV(txData.pin);
    --txData.bitsLeft;
}
