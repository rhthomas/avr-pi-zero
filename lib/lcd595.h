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

#ifndef LCD595_H_
#define LCD595_H_

// libraries
#include <avr/io.h>
#include <util/delay.h>
#include "sipo.h"

#define LCD_CMD_DELAY 2 // [ms] TEST THIS VALUE! MIGHT NEED ADJUSTING

// global variable
volatile uint8_t sipoData=0;

// controller commands
#define CLEAR      0x01 // clear lcd
#define HOME       0x02 // return cursor home
#define ENTRY      0x06 // shift cursor from left to right on read/write
#define DISPLAY_ON 0x0C // display on, cursor off, dont blink
#define SET_4_BIT  0x28 // 4-bit data, 2-line display, 5*7 font
#define SET_CURSOR 0x80 // set cursor position

// user functions
void setup_lcd(void); // setup parameters for the lcd. see datasheet
void lcd_print(const char *text); // displays string on lcd
void lcd_cursor(uint8_t x, uint8_t y); // move cursor on display. 0<=x<=15 0<=y<=1
void lcd_clear(void); // clears the display
void lcd_home(void); // sends cursor back to 0,0 without clearing

// low-level functions
void lcd_cmd(uint8_t cmd); // sends command to lcd by setting R/S low
void lcd_char(uint8_t data); // sends character to lcd by setting R/S high
uint8_t arrange_data(uint8_t data, uint8_t rs); // arranges data in format for '595
void pulse_enable(void); // pulse enable bit on '595 connected to lcd
void wait_busy(void); // delay until free

#endif