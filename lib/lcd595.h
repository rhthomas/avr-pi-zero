// file    : lcd595.h
// author  : Rhys Thomas
// created : 2017-01-06
/* Header to drive HD44780 lcd controller using '595
 * sipo chip. Derived from Bruce Hall's lcd code at
 * http://w8bh.net/avr/AvrLCD1.pdf
 * IN PROGRESS!
 */

#include <avr/io.h>
#include "sipo.h"

typedef uint8_t byte; // quicker than uint8_t

// lcd pins
//#define LCD_RS PB0
//#define LCD_E  PB1
//#define DATA4  PB2
//#define DATA5  PB3
//#define DATA6  PB4
//#define DATA7  PB5

// 595 to lcd connections
// Q0 - x
// Q1 - x
// Q2 - LCD_RS
// Q3 - LCD_E
// Q4 - DATA4
// Q5 - DATA5
// Q6 - DATA6
// Q7 - DATA7

// controller commands
#define CLEAR     0x01
#define SETCURSOR 0x80

// send top 4 bits, pulse enable, send bottom 4 bits
void pulse_lcd_enable(void)
{
    //PORTB |= _BV(LCD_E);
    shift_out(0x08); // 0b0000.10xx
    _delay_us(40); // check datasheet for this
    //PORTB &= ~_BV(LCD_E);
    shift_out(0x00);
}

// sends 4 bits of a byte message
void send_nibble(byte data, byte cmd)
{
    //PORTB &= 0xC3; // 11000011 = clear 4 data lines
    shift_out(0x00); // 0000.00xx = clear 4 data lines
    byte shift;
    //if (data & (1<<4)) PORTB |= _BV(DATA4);
    //if (data & (1<<5)) PORTB |= _BV(DATA5);
    //if (data & (1<<6)) PORTB |= _BV(DATA6);
    //if (data & (1<<7)) PORTB |= _BV(DATA7);
    shift_out((data & 0xF0) | (cmd & 0x04)); // if cmd, R/S is low
    pulse_lcd_enable();
}

// send byte of data. calls send_nibble to get upper/lower 4 bits
void send_byte(byte data)
{
    send_nibble(data); // upper 4 bits
    send_nibble(data<<=4); // lower 4 bits
}

// sends command to lcd by setting R/S low
void lcd_cmd(byte cmd)
{
    //PORTB &= ~_BV(LCD_RS); // R/S line low = command data
    send_byte(cmd,0);
}

// sends character to lcd by setting R/S high
void lcd_char(byte ch)
{
    //PORTB |= _BV(LCD_RS); // R/S line high = character data
    //shift_out(0x04); // 0b0000.01xx
    send_byte(ch,1);
}

// setup parameters for the lcd. see datasheet
void setup_lcd(void)
{
	// TODO: convert binarys!
	// Bruce used xx.data[7:4].E.RS
    lcd_cmd(0x33); // 0b0011.0011 // init the controller
    lcd_cmd(0x32); // 0b0011.0010 // set to 4-bit input mode
    lcd_cmd(0x28); // 0b0010.1000 // 2 line, 5x7 matrix
    lcd_cmd(0x0C); // 0b0000.1100 // turn cursor off, on with 0x0E
    lcd_cmd(0x06); // 0b0000.0110 // cursor direction = right
    lcd_cmd(0x01); // 0b0000.0001 // start with clear display
    _delay_ms(3);  // wait for lcd to initialise
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
    lcd_cmd(SETCURSOR);
}

// move curor on diplay. 0<=x<=11 0<=y<=1
void lcd_cursor(byte x, byte y)
{
    byte addr = 0; // line 0 begins at 0x00
    if(y==1) addr = 0x40; // line 1 begins at 0x40
    lcd_cmd(SETCURSOR+addr+x); // update cursor with x,y pos
}

// displays string on lcd
void lcd_print(const char *text)
{
    // do until /0 character
    while(*text) {
    	// auto-increment array pointer
        lcd_char(*text++);
    }
}
