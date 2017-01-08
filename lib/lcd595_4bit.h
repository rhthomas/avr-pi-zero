// file    : lcd595.h
// author  : Rhys Thomas
// created : 2017-01-06
/* Header to drive HD44780 lcd controller using '595
 * sipo chip.
 * http://web.alfredstate.edu/weimandn/programming/lcd/ATmega328/LCD_code_gcc_4d.html
 * http://w8bh.net/avr/AvrLCD1.pdf
 * NOT YET WORKING
 */

#include "sipo.h"

// 595 to lcd connections
// Q0 - LCD_RS
// Q1 - LCD_E
// Q2 - DATA4
// Q3 - DATA5
// Q4 - DATA6
// Q5 - DATA7
// Q6 - nc
// Q7 - nc

// enable and RS pins
#define LCD_RS 0x01
#define LCD_E  0x02

// controller commands
#define CLEAR   0b00000001 // clear lcd
#define HOME    0b00000010 // return cursor home
#define ENTRY   0b00000110 // shift cursor from left to right on read/write
#define DISPOFF 0b00001000 // turn display off
#define DISPON  0b00001100 // display on, cursor off, dont blink
#define FUNCRST 0b00110000 // reset the lcd
#define SET4BIT 0b00101000 // 4-bit data, 2-line display, 5*7 font
#define SETCURS 0b10000000 // set cursor position

// sends 4 bits of a byte message
void send_nibble(uint8_t data, uint8_t isChar)
{
    // creates bit structure for '595
    // eg. data = 0b1011.0100 -> 0b1011.0000 -> 0b0010.1100
    uint8_t toShift = ((data & 0xF0) >> 2) | isChar;

    // pulse the E pin on the lcd with the data
    shift_out(toShift | LCD_E); // enable pin high
    _delay_us(40);
    shift_out(toShift); // enable pin low
    _delay_us(40);
}

// sends command to lcd by setting R/S low
void lcd_cmd(uint8_t cmd)
{
    // RS LOW
    send_nibble(cmd,0);
    send_nibble(cmd<<=4,0);
}

// sends character to lcd by setting R/S high
void lcd_char(uint8_t data)
{
    // RS HIGH
    send_nibble(data,1);
    send_nibble(data<<=4,1);
}

// setup parameters for the lcd. see datasheet
void setup_lcd(void)
{
    _delay_ms(100); // at least 40ms to set up display

    // 4-bit mode setup routine
    lcd_cmd(FUNCRST);
    _delay_ms(10); // 4.1ms min
    lcd_cmd(FUNCRST);
    _delay_us(200); // 100us min
    lcd_cmd(FUNCRST);
    _delay_us(200);
    lcd_cmd(SET4BIT); // set 4-bit mode
    _delay_us(80); // 40us min

    lcd_cmd(DISPOFF); // turn display off
    _delay_us(80); // 40us min
    lcd_cmd(CLEAR); // clear display ram
    _delay_ms(4); // 1.64ms min
    lcd_cmd(ENTRY); // set desired shift characteristics
    _delay_us(80); // 40us min

    lcd_cmd(DISPON); // turn display on
    _delay_us(80); // 40us min
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
