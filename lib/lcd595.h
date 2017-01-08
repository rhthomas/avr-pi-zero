// file    : lcd595.h
// author  : Rhys Thomas
// created : 2017-01-06
/* Header to drive HD44780 lcd controller using '595
 * sipo chip. Derived from Bruce Hall's lcd code at
 * http://w8bh.net/avr/AvrLCD1.pdf
 * WORK IN PROGRESS! NOT YET TESTED
 * it compiles! now need to hook up lcd to see if it actually works
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

// controller commands
#define SETCURSOR 0x80

// sends 4 bits of a byte message
void send_nibble(uint8_t data, uint8_t cmd)
{
    // is this necessary? will have to test
    //PORTB &= 0xC3; // 1100.0011 = clear 4 data lines
    //shift_out(0x03); // xx00.0011 = clear 4 data lines

    //if (data & (1<<4)) PORTB |= _BV(DATA4);
    //if (data & (1<<5)) PORTB |= _BV(DATA5);
    //if (data & (1<<6)) PORTB |= _BV(DATA6);
    //if (data & (1<<7)) PORTB |= _BV(DATA7);

    // creates bit structure for '595
    // eg. data = 0b1011.0100 -> 0b1011.0000 -> 0b0010.1100
    // |cmd  adds the bit for R/S assertion
    uint8_t toShift = ((data & 0xF0) >> 2) | cmd;

    // pulse the E pin on the lcd with the data
    shift_out(toShift | 0x02); // set E high w/ data
    _delay_us(40); // probably dont need this since shift_out is quite slow
    shift_out(toShift & ~(0x02)); // set E low w/ data
}

// send byte of data. calls send_nibble to get upper/lower 4 bits
void send_byte(uint8_t data, uint8_t cmd)
{
    send_nibble(data,cmd); // upper 4 bits
    send_nibble(data<<=4,cmd); // lower 4 bits
}

// sends command to lcd by setting R/S low
void lcd_cmd(uint8_t cmd)
{
    //PORTB &= ~_BV(LCD_RS); // R/S line low = command data
    send_byte(cmd,0);
}

// sends character to lcd by setting R/S high
void lcd_char(uint8_t ch)
{
    //PORTB |= _BV(LCD_RS); // R/S line high = character data
    send_byte(ch,1);
}

// setup parameters for the lcd. see datasheet
void setup_lcd(void)
{
    // data structure: xx.data[7:4].E.RS
    lcd_cmd(0x33); // 0bxx11.0011 // init the controller
    lcd_cmd(0x32); // 0bxx11.0010 // set to 4-bit input mode
    lcd_cmd(0x28); // 0bxx10.1000 // 2 line, 5x7 matrix
    lcd_cmd(0x0C); // 0bxx00.1100 // turn cursor off, on with 0x0E
    lcd_cmd(0x06); // 0bxx00.0110 // cursor direction = right
    lcd_cmd(0x01); // 0bxx00.0001 // start with clear display
    _delay_ms(3);  // wait for lcd to initialise
}

// clears the display
void lcd_clear(void)
{
    lcd_cmd(0x01);
    _delay_ms(3); // wait for lcd to process cmd
}

// sends cursor back to 0,0 without clearing
void lcd_home(void)
{
    lcd_cmd(SETCURSOR);
}

// move curor on diplay. 0<=x<=11 0<=y<=1
void lcd_cursor(uint8_t x, uint8_t y)
{
    uint8_t addr = 0; // line 0 begins at 0x00
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
