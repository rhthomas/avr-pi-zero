// file    : adc.c
// author  : Rhys Thomas
// created : 2016-12-12
/* testing adc operation of attiny
 * using a potentiometer and some leds
 */

#define F_CPU 1000000L // 1MHz internal clock
#include <avr/io.h>
#include <util/delay.h>
#include "sipo.h"
#include <math.h>

void initAdc(void)
{
    ADMUX = 2; // using ADC2 on PB3
    // enable adc, prescaler = 64
    ADCSRA |= _BV(ADEN) | _BV(ADPS1) | _BV(ADPS2);
}

uint16_t readAdc(void)
{
    // start conversion
    ADCSRA |= _BV(ADSC);
    // wait for conversion to complete
    while(!(ADCSRA & _BV(ADIF))){};
    ADC = (ADCH << 8) | ADCL;
    return ADC;
}

int main(void)
{
    uint8_t result,ledNum;

    initAdc();
    shiftInit();

    for(;;) {
        /* drop the last 2 bits of the
         * adc reading since we only have
         * 8 leds on the shift register
         */
        result = readAdc() >> 2;

        // max led to light up
        ledNum = log(result)/log(2);
        // shift 1 to the led to turn on
        shiftOut(1<<ledNum);
        _delay_ms(100);
    }

    return 0;
}
