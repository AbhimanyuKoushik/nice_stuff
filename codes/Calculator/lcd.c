#include "lcd.h"
#include <util/delay.h>

// Define LCD pins
#define RS 13      // Register Select pin
#define EN 12      // Enable pin
#define D4 A2      // Data pin 4
#define D5 A3      // Data pin 5
#define D6 A4      // Data pin 6
#define D7 A5      // Data pin 7

// Define keypad pins
#define ROW1 1     // D1
#define ROW2 2     // D2
#define ROW3 3     // D3
#define ROW4 4     // D4
#define ROW5 5     // D5

#define COL1 7     // D7
#define COL2 8     // D8
#define COL3 9     // D9
#define COL4 10    // D10
#define COL5 11    // D11

// Define macros for pin manipulation
#define SetBit(PORT, BIT) ((PORT) |= (1 << (BIT)))
#define ClearBit(PORT, BIT) ((PORT) &= ~(1 << (BIT)))

// Convert Arduino pin numbers to AVR port and pin
#define RS_PORT PORTB
#define RS_PIN 5    // Arduino pin 13 is PB5
#define EN_PORT PORTB
#define EN_PIN 4    // Arduino pin 12 is PB4
#define D4_PORT PORTC
#define D4_PIN 2    // Arduino pin A2 is PC2
#define D5_PORT PORTC
#define D5_PIN 3    // Arduino pin A3 is PC3
#define D6_PORT PORTC
#define D6_PIN 4    // Arduino pin A4 is PC4
#define D7_PORT PORTC
#define D7_PIN 5    // Arduino pin A5 is PC5

// Define direction registers
#define RS_DDR DDRB
#define EN_DDR DDRB
#define D4_DDR DDRC
#define D5_DDR DDRC
#define D6_DDR DDRC
#define D7_DDR DDRC

// Keypad row port and pin definitions
#define ROW1_PORT PORTD
#define ROW1_PIN 1     // D1 is PD1
#define ROW2_PORT PORTD
#define ROW2_PIN 2     // D2 is PD2
#define ROW3_PORT PORTD
#define ROW3_PIN 3     // D3 is PD3
#define ROW4_PORT PORTD
#define ROW4_PIN 4     // D4 is PD4
#define ROW5_PORT PORTD
#define ROW5_PIN 5     // D5 is PD5

// Keypad row direction registers
#define ROW1_DDR DDRD
#define ROW2_DDR DDRD
#define ROW3_DDR DDRD
#define ROW4_DDR DDRD
#define ROW5_DDR DDRD

// Keypad column port and pin definitions
#define COL1_PORT PORTD
#define COL1_PIN 7     // D7 is PD7
#define COL2_PORT PORTB
#define COL2_PIN 0     // D8 is PB0
#define COL3_PORT PORTB
#define COL3_PIN 1     // D9 is PB1
#define COL4_PORT PORTB
#define COL4_PIN 2     // D10 is PB2
#define COL5_PORT PORTB
#define COL5_PIN 3     // D11 is PB3

// Keypad column direction registers
#define COL1_DDR DDRD
#define COL2_DDR DDRB
#define COL3_DDR DDRB
#define COL4_DDR DDRB
#define COL5_DDR DDRB

// Keypad column input registers
#define COL1_PIN_REG PIND
#define COL2_PIN_REG PINB
#define COL3_PIN_REG PINB
#define COL4_PIN_REG PINB
#define COL5_PIN_REG PINB

// LCD commands
#define LCD_CLEAR 0x01
#define LCD_HOME 0x02
#define LCD_ENTRY_MODE 0x06
#define LCD_DISPLAY_ON 0x0C
#define LCD_FUNCTION_4BIT_2LINE 0x28
#define LCD_SET_CURSOR 0x80


void lcd_init(void) {
    // Set all LCD pins as output
    SetBit(RS_DDR, RS_PIN);
    SetBit(EN_DDR, EN_PIN);
    SetBit(D4_DDR, D4_PIN);
    SetBit(D5_DDR, D5_PIN);
    SetBit(D6_DDR, D6_PIN);
    SetBit(D7_DDR, D7_PIN);
    
    // Wait for LCD to initialize
    _delay_ms(20);
    
    // Initialize LCD in 4-bit mode
    lcd_command(0x33);    // Special case of 4-bit initialization
    lcd_command(0x32);    // Special case of 4-bit initialization
    lcd_command(LCD_FUNCTION_4BIT_2LINE);  // 4-bit mode, 2 lines, 5x8 font
    lcd_command(LCD_DISPLAY_ON);           // Display on, cursor off
    lcd_command(LCD_ENTRY_MODE);           // Increment cursor position
    lcd_command(LCD_CLEAR);                // Clear display
    _delay_ms(2);                          // Clear display needs extra time
}

void lcd_command(unsigned char cmd) {
    // Send higher nibble
    ClearBit(RS_PORT, RS_PIN);    // RS=0 for command
    
    // Send higher 4 bits
    if(cmd & 0x80) SetBit(D7_PORT, D7_PIN); else ClearBit(D7_PORT, D7_PIN);
    if(cmd & 0x40) SetBit(D6_PORT, D6_PIN); else ClearBit(D6_PORT, D6_PIN);
    if(cmd & 0x20) SetBit(D5_PORT, D5_PIN); else ClearBit(D5_PORT, D5_PIN);
    if(cmd & 0x10) SetBit(D4_PORT, D4_PIN); else ClearBit(D4_PORT, D4_PIN);
    
    // Generate enable pulse
    SetBit(EN_PORT, EN_PIN);
    _delay_us(1);
    ClearBit(EN_PORT, EN_PIN);
    _delay_us(200);
    
    // Send lower nibble
    if(cmd & 0x08) SetBit(D7_PORT, D7_PIN); else ClearBit(D7_PORT, D7_PIN);
    if(cmd & 0x04) SetBit(D6_PORT, D6_PIN); else ClearBit(D6_PORT, D6_PIN);
    if(cmd & 0x02) SetBit(D5_PORT, D5_PIN); else ClearBit(D5_PORT, D5_PIN);
    if(cmd & 0x01) SetBit(D4_PORT, D4_PIN); else ClearBit(D4_PORT, D4_PIN);
    
    // Generate enable pulse
    SetBit(EN_PORT, EN_PIN);
    _delay_us(1);
    ClearBit(EN_PORT, EN_PIN);
    _delay_ms(2);
}

void lcd_char(unsigned char data) {
    // Send higher nibble
    SetBit(RS_PORT, RS_PIN);    // RS=1 for data
    
    // Send higher 4 bits
    if(data & 0x80) SetBit(D7_PORT, D7_PIN); else ClearBit(D7_PORT, D7_PIN);
    if(data & 0x40) SetBit(D6_PORT, D6_PIN); else ClearBit(D6_PORT, D6_PIN);
    if(data & 0x20) SetBit(D5_PORT, D5_PIN); else ClearBit(D5_PORT, D5_PIN);
    if(data & 0x10) SetBit(D4_PORT, D4_PIN); else ClearBit(D4_PORT, D4_PIN);
    
    // Generate enable pulse
    SetBit(EN_PORT, EN_PIN);
    _delay_us(1);
    ClearBit(EN_PORT, EN_PIN);
    _delay_us(200);
    
    // Send lower nibble
    if(data & 0x08) SetBit(D7_PORT, D7_PIN); else ClearBit(D7_PORT, D7_PIN);
    if(data & 0x04) SetBit(D6_PORT, D6_PIN); else ClearBit(D6_PORT, D6_PIN);
    if(data & 0x02) SetBit(D5_PORT, D5_PIN); else ClearBit(D5_PORT, D5_PIN);
    if(data & 0x01) SetBit(D4_PORT, D4_PIN); else ClearBit(D4_PORT, D4_PIN);
    
    // Generate enable pulse
    SetBit(EN_PORT, EN_PIN);
    _delay_us(1);
    ClearBit(EN_PORT, EN_PIN);
    _delay_ms(2);
}

void lcd_string(char *str) {
    while(*str) {
        lcd_char(*str++);
    }
}

void lcd_clear(void) {
    lcd_command(LCD_CLEAR);
    _delay_ms(2);  // Clear display command needs extra time
}