// file    : lcd595_8bit.h
// author  : Rhys Thomas
// created : 2017-01-06
/* Header to drive HD44780 lcd controller using '595 sipo chip
 * to drive D7-D0. LCD_E and LCD_RS still controlled by microcontroller.
 * Driving control pins by shift register was casuing some weird behaviour.
 */

#include "sipo.h"

// '595 to lcd connections
// Qn - DBn

// control pins
#define LCD_E _BV(PB3)
#define LCD_RS _BV(PB4)

// controller commands
#define CLEAR   0b00000001 // clear lcd
#define HOME    0b00000010 // return cursor home
#define ENTRY   0b00000110 // shift cursor from left to right on read/write
#define DISPOFF 0b00001000 // turn display off
#define DISPON  0b00001100 // display on, cursor off, dont blink
#define FUNCRST 0b00110000 // reset the lcd
#define SET4BIT 0b00101000 // 4-bit data, 2-line display, 5*7 font
#define SETCURS 0b10000000 // set cursor position

void pulse_enable(void)
{
    PORTB |= LCD_E;
    _delay_us(40);
    PORTB &= ~LCD_E;
}

// sends command to lcd by setting R/S low
void lcd_cmd(uint8_t cmd)
{
    PORTB &= ~(LCD_E | LCD_RS); // make sure E and RS are low
    // write data to shift register
    shift_out(cmd);
    // send to lcd by pulsing LCD
    pulse_enable();
}

// sends character to lcd by setting R/S high
void lcd_char(uint8_t data)
{
    PORTB &= ~LCD_E; // make sure E is low
    PORTB |= LCD_RS; // RS high for charater transfer
    // write data to shift register
    shift_out(data);
    // send to lcd by pulsing LCD
    pulse_enable();
}

// setup parameters for the lcd. see datasheet
void setup_lcd(void)
{
    _delay_ms(100); // at least 40ms to set up display

    // set control pins as outputs, init low
    DDRB |= (LCD_E | LCD_RS);
    PORTB &= ~(LCD_E | LCD_RS);

    lcd_cmd(0x38); // 8-bit mode
    lcd_cmd(0x01); // clear screen
    lcd_cmd(0x06); // cursor increments automatically
    lcd_cmd(0x0C|0x08); // display on, no cursor (0x14)
}

// clears the display
void lcd_clear(void)
{
    lcd_cmd(CLEAR);
    _delay_ms(3); // wait for lcd to process cmd
}

// sends cursor back to 0,0 without clearing
void lcd_home(void)
{
    lcd_cmd(SETCURS);
}

// move curor on diplay. 0<=x<=11 0<=y<=1
void lcd_cursor(uint8_t x, uint8_t y)
{
    uint8_t addr = 0; // line 0 begins at 0x00
    if(y==1) addr = 0x40; // line 1 begins at 0x40
    lcd_cmd(SETCURS+addr+x); // update cursor with x,y pos
}

// displays string on lcd
void lcd_print(const char *text)
{
    // do until /0 character
    while(*text) {
    	// auto-increment array pointer
        lcd_char(*text++);
        _delay_us(80); // 40us min
    }
}
