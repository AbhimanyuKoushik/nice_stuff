#include "keypad.h"
#include <util/delay.h>
#include <avr/pgmspace.h>

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

// Keypad layout (5x5 matrix)
char keypad[5][5] = {
    {'S', 'A', 'C', 'B', '='},
    {'1', '2', '3', '4', '5'},
    {'6', '7', '8', '9', '0'},
    {'+', '-', '*', '/', '^'},
    {'s', 'c', 't', 'p', 'e'}
};

const char* const keypad_alpha[5][5] PROGMEM  = {
    {"sin", "cos", "tan", "^", "BS"},
    {"log", "ln", "e^", "√", "("},
    {"π", "x²", "x³", "1/x", ")"},
    {"EXP", "ANS", "M+", "M-", "MR"},
    {"asin", "acos", "atan", "y^x", "CLR"},
};

void keypad_init(void) {
    // Set row pins as outputs
    SetBit(ROW1_DDR, ROW1_PIN);
    SetBit(ROW2_DDR, ROW2_PIN);
    SetBit(ROW3_DDR, ROW3_PIN);
    SetBit(ROW4_DDR, ROW4_PIN);
    SetBit(ROW5_DDR, ROW5_PIN);
    
    // Set column pins as inputs with pull-up resistors
    ClearBit(COL1_DDR, COL1_PIN);
    ClearBit(COL2_DDR, COL2_PIN);
    ClearBit(COL3_DDR, COL3_PIN);
    ClearBit(COL4_DDR, COL4_PIN);
    ClearBit(COL5_DDR, COL5_PIN);
    
    // Enable pull-up resistors for columns
    SetBit(COL1_PORT, COL1_PIN);
    SetBit(COL2_PORT, COL2_PIN);
    SetBit(COL3_PORT, COL3_PIN);
    SetBit(COL4_PORT, COL4_PIN);
    SetBit(COL5_PORT, COL5_PIN);
    
    // Initially set all rows to HIGH
    SetBit(ROW1_PORT, ROW1_PIN);
    SetBit(ROW2_PORT, ROW2_PIN);
    SetBit(ROW3_PORT, ROW3_PIN);
    SetBit(ROW4_PORT, ROW4_PIN);
    SetBit(ROW5_PORT, ROW5_PIN);
}

char keypad_scan(void) {
    // Variable to store the pressed key
    char pressed_key = 0;
    
    // Iterate through each row
    for (int r = 0; r < 5; r++) {
        // Set the current row to LOW, keep others HIGH
        if (r == 0) ClearBit(ROW1_PORT, ROW1_PIN); else SetBit(ROW1_PORT, ROW1_PIN);
        if (r == 1) ClearBit(ROW2_PORT, ROW2_PIN); else SetBit(ROW2_PORT, ROW2_PIN);
        if (r == 2) ClearBit(ROW3_PORT, ROW3_PIN); else SetBit(ROW3_PORT, ROW3_PIN);
        if (r == 3) ClearBit(ROW4_PORT, ROW4_PIN); else SetBit(ROW4_PORT, ROW4_PIN);
        if (r == 4) ClearBit(ROW5_PORT, ROW5_PIN); else SetBit(ROW5_PORT, ROW5_PIN);
        
        // Small delay to allow signals to stabilize
        _delay_us(10);
        
        // Check each column for the current row
        if (!(COL1_PIN_REG & (1 << COL1_PIN))) {
            pressed_key = keypad[r][0];
        }
        if (!(COL2_PIN_REG & (1 << COL2_PIN))) {
            pressed_key = keypad[r][1];
        }
        if (!(COL3_PIN_REG & (1 << COL3_PIN))) {
            pressed_key = keypad[r][2];
        }
        if (!(COL4_PIN_REG & (1 << COL4_PIN))) {
            pressed_key = keypad[r][3];
        }
        if (!(COL5_PIN_REG & (1 << COL5_PIN))) {
            pressed_key = keypad[r][4];
        }
        
        // If a key was pressed, restore row states and return the key
        if (pressed_key != 0) {
            // Restore all rows to HIGH
            SetBit(ROW1_PORT, ROW1_PIN);
            SetBit(ROW2_PORT, ROW2_PIN);
            SetBit(ROW3_PORT, ROW3_PIN);
            SetBit(ROW4_PORT, ROW4_PIN);
            SetBit(ROW5_PORT, ROW5_PIN);
            
            return pressed_key;
        }
    }
    
    // No key was pressed, restore row states
    SetBit(ROW1_PORT, ROW1_PIN);
    SetBit(ROW2_PORT, ROW2_PIN);
    SetBit(ROW3_PORT, ROW3_PIN);
    SetBit(ROW4_PORT, ROW4_PIN);
    SetBit(ROW5_PORT, ROW5_PIN);
    
    return 0;
}