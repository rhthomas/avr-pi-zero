// file    : uart.c
// author  : Rhys Thomas
// created : 2016-12-13
/* serial transmission from attiny adapted from
 * Ido Gendel at http://pastebin.com/cKn6G2mL to
 * work with a 1MHz clock. 9600,8,N,1
 * screen /dev/ttyAMA0 9600
 */

#define F_CPU 1000000L // 1MHz internal clock
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

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

int main(void)
{
    initSerial(PB0);
    sei(); // enable interrupts

    uint8_t count=0;
    char str[80]; // store sprintf string here

    for(;;) {
        _delay_ms(100);
        // screen wants \r\n
        sprintf(str,"count: %i\r\n",count);
        sendStrBySerial(str);
        count++;
    }

    return 0;
}
