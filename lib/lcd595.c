// file    : lcd595.c
// author  : Rhys Thomas
// created : 2017-01-09
/* Library to drive HD44780 lcd controller using '595
 * sipo chip. Adapted from Eka Puji Widiyanto's lcd_lib.c
 * at https://github.com/ekapujiw2002/ex4-avr-gcc-library/
 */

#include "lcd595.h"

#define LCD_CMD_DELAY 2 // [ms] ADJUST WITH YOUR OWN LCD!!!

// global variable
volatile uint8_t sipoData=0;

void setup_lcd(void)
{
    _delay_ms(50); // wait for lcd to warm up

    // data,latch,clock
    setup_sipo(PB0,PB1,PB2);
    shift_out(0x00);

    _delay_ms(50);

    sipo_data = arrange_data(0x18,0); // DB4=DB5=1, RS=0

    shift_out(sipo_data);
    pulse_enable();
    _delay_ms(10);
    shift_out(sipo_data);
    pulse_enable();
    _delay_ms(5);
    shift_out(sipo_data);
    pulse_enable();
    _delay_ms(5);

    sipo_data = arrange_data(0x10,0); // DB5=1, RS=0

    shift_out(sipo_data);
    pulse_enable();
    _delay_ms(5);

    lcd_cmd(SET_4_BIT); // 4-bit, 2 line, 5x7 font
    lcd_cmd(DISPLAY_ON); // display on cursor off
    lcd_cmd(ENTRY); // auto inc, display shift off
    lcd_cmd(SET_CURSOR); // cursor home
    lcd_cmd(CLEAR); // clear lcd (test this!)
}

void lcd_print(const char *text)
{
    // do until /0 character
    while(*text) {
        // auto-increment array pointer
        lcd_char(*text++);
    }
}

void lcd_cursor(uint8_t x, uint8_t y)
{
    uint8_t addr = 0; // line 0 begins at 0x00
    if(y==1) addr = 0x40; // line 1 begins at 0x40
    lcd_cmd(SET_CURSOR+addr+x); // update cursor with x,y pos
}

void lcd_clear(void)
{
    lcd_cmd(CLEAR);
    _delay_ms(3); // wait for lcd to process cmd
}

void lcd_home(void)
{
    lcd_cmd(HOME);
}

void lcd_cmd(uint8_t cmd)
{
    wait_busy();

    // send high byte, RS=E=0
    sipo_data = arrange_data(cmd,0);
    shift_out(sipo_data);
    pulse_enable();

    // send low byte, RS=E=0
    sipo_data = arrange_data(cmd<<=4,0);
    shift_out(sipo_data);
    pulse_enable();
}

void lcd_char(uint8_t data)
{
    wait_busy();

    // send high byte, RS=1, E=0
    sipo_data = arrange_data(data,1);
    shift_out(sipo_data);
    pulse_enable();

    // send low byte, RS=1, E=0
    sipo_data = arrange_data(data<<=4,1);
    shift_out(sipo_data);
    pulse_enable();
}

uint8_t arrange_data(uint8_t data, uint8_t rs)
{
    // data = 1101.0111 -> 1101.0000 -> 0011.0100 -> 0011.010r
    return ((data & 0xF0) >> 2) | rs;
}

void pulse_enable(void)
{
    shift_out(sipo_data | 0x02);
    _delay_us(1);
    shift_out(sipo_data & ~(0x02));
}

void wait_busy(void)
{
    _delay_ms(CMD_DELAY);
}
