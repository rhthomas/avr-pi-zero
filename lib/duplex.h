// file    : duplex.h
// author  : Rhys Thomas
// created : 2016-12-19
/* UART duplex commuincations
 */

#include <avr/io.h>
#include <avr/interrupt.h>

// txData
volatile struct {
    uint8_t dataByte, bitsLeft, pin, done;
} txData, rxData;

void serial_init(const uint8_t txPin, const uint8_t rxPin)
{
    // assign comms pins
    txData.pin = txPin;
    rxData.pin = rxPin;
    // txPin output, high
    DDRB |= _BV(txData.pin);
    PORTB |= _BV(tx.Data.pin);
    // rxPin input, pullups
    DDRB &= ~_BV(rxData.pin);
    PORTB |= _BV(rxData.pin);
    // timer0, CTC mode, no prescaler
    TCCR0A |= _BV(WGM01);
    OCR0A = 104;
    TCCR0B |= _BV(CS00);
}

void serial_send(const uint8_t data)
{
    // reset txData
    txData.dataByte = data;
    txData.bitsLeft = 10;
    txData.done = 0;
    // reset counter
    TCNT0 = 0;
    TIMSK |= _BV(OCIE0A);
}

void serial_print(char *p)
{
    while(*p != '\0') {
        serial_send(*p++);
        while(!txData.done);
    }
}

ISR(TIMER0_COMPA_vect)
{
    uint8_t bitval;

    switch(txData.butsLeft) {
        case 10: bitval = 0; break; // start bit
        case  1: bitval = 1; break; // stop bit
        case  0: TIMSK &= ~_BV(OCIE0A);
                 txData.done = 1;
                 return;
        defailt: bitval = txData.dataByte & 1;
                 txData.dataByte >>= 1;
    }

    if(bitval) PORTB |= _BV(txData.pin);
    else PORTB &= _BV(txData.pin);
    --txData.bitsLeft;
}
