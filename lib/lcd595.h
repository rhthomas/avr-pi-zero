// file    : lcd595.h
// author  : Rhys Thomas
// created : 2017-01-09
/* Library to drive HD44780 lcd controller using '595
 * sipo chip. Adapted from Eka Puji Widiyanto's lcd_lib.c
 * at https://github.com/ekapujiw2002/ex4-avr-gcc-library/
 */

// 595 to lcd connections
// Q0 - LCD_RS
// Q1 - LCD_E
// Q2 - DATA4
// Q3 - DATA5
// Q4 - DATA6
// Q5 - DATA7
// Q6 - nc
// Q7 - nc

// libraries
#include <avr/io.h>
#include "sipo.h"

// global variable
volatile uint8_t sipoData=0;

// controller commands
#define CLEAR       0x01 // clear lcd
#define HOME        0x02 // return cursor home
#define ENTRY       0x06 // shift cursor from left to right on read/write
#define DISPLAY_ON  0x0C // display on, cursor off, dont blink
#define DISPLAY_OFF 0x08 // display off
#define SET_4_BIT   0x28 // 4-bit data, 2-line display, 5*7 font
#define SET_CURSOR  0x80 // set cursor position

// function declarations
void setup_lcd(void);
void lcd_print(const char *text);
void lcd_cursor(uint8_t x, uint8_t y);
void lcd_clear(void);
void lcd_home(void);
void lcd_cmd(uint8_t cmd);
void lcd_char(uint8_t data);
uint8_t arrange_data(uint8_t data, uint8_t rs);
void pulse_enable(void);
void wait_busy(void);

// user functions
void setup_lcd(void)
{
    _delay_ms(50); // wait for lcd to warm up
    // data,latch,clock
    setup_sipo(PB0,PB1,PB2);
    shift_out(0x00);
     _delay_ms(50);

    // setup for 4-bit mode
    send_nibble(0x03);
    _delay_ms(4.5);
    send_nibble(0x03);
    _delay_ms(4.5);
    send_nibble(0x03);
    _delay_us(150);

    // 4-bit bus width
    send_nibble(0x02);
    _delay_us(150);

    lcd_cmd(SET_4_BIT);
    _delay_us(40);

    lcd_cmd(DISPLAY_OFF);
    lcd_cmd(CLEAR);
    lcd_cmd(ENTRY);
    lcd_cmd(DISPLAY_ON);
}

void send_nibble(uint8_t code)
{
    sipoData = arrange_data(code,0);
    shift_out(sipoData);
    pulse_enable();
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
}

void lcd_home(void)
{
    lcd_cmd(HOME);
}

void lcd_cmd(uint8_t cmd)
{
    // send high byte, RS=E=0
    sipoData = arrange_data(cmd,0);
    shift_out(sipoData);
    pulse_enable();

    // send low byte, RS=E=0
    sipoData = arrange_data(cmd<<=4,0);
    shift_out(sipoData);
    pulse_enable();

    wait_busy();
}

void lcd_char(uint8_t data)
{
    // send high byte, RS=1, E=0
    sipoData = arrange_data(data,1);
    shift_out(sipoData);
    pulse_enable();

    // send low byte, RS=1, E=0
    sipoData = arrange_data(data<<=4,1);
    shift_out(sipoData);
    pulse_enable();

    wait_busy();
}

uint8_t arrange_data(uint8_t data, uint8_t rs)
{
    // data = 1101.0111 -> 1101.0000 -> 0011.0100 -> 0011.010r
    return ((data & 0xF0) >> 2) | rs;
}

void pulse_enable(void)
{
    shift_out(sipoData | 0x02);
    _delay_us(40); // needs to be held for at least 37us
    shift_out(sipoData & ~(0x02));
    _delay_us(40);
}

void wait_busy(void)
{
    _delay_ms(2);
}
