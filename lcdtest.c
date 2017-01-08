// file    : lcdtest.c
// author  : Rhys Thomas
// created : 2017-01-08
/* Testing lcd595.h library.
 */

#define F_CPU 1000000L // 1MHz internal clock
#include <avr/io.h>
#include <util/delay.h>
#include "lib/lcd595_8bit.h"

int main(void)
{
    // data,latch,clock,num
    setup_sipo(PB0,PB1,PB2);
    setup_lcd();

    lcd_print("Hello");

    for(;;) {}
    return 0;
}
