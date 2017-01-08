// file    : lcdtest.c
// author  : Rhys Thomas
// created : 2017-01-08
/* Testing lcd595.h library.
 */

#define F_CPU 1000000L // 1MHz internal clock
#include <avr/io.h>
#include <util/delay.h>
#include "lib/lcd595.h"

int main(void)
{
    // data,latch,clock,num
    setup_sipo(PB0,PB1,PB2,1);
    setup_lcd();

    _delay_ms(1000);
    lcd_clear();
    lcd_print("Hello");

    for(;;) {}
    return 0;
}
