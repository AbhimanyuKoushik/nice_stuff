#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

void handle_shift_mode(char *key);
void handle_alpha_mode(char *key);
void lcd_init(void);
void lcd_command(unsigned char cmd);
void lcd_char(unsigned char data);
void lcd_string(char *str);
void lcd_clear(void);
void keypad_init(void);
char keypad_scan(void);
void evaluate_expression(char *expression);
float perform_operation(float a, float b, char op);
float parse_number(char **expr);
float perform_exponentiation(float a, float b);
float perform_operation_extended(float a, float b, char op);
float calculate_sin_degrees(float angle_in_degrees);
float calculate_cos_degrees(float angle_in_degrees);
float calculate_tan_degrees(float angle_in_degrees);
void display_trig_result(char *func_name, float angle, float result);
void extended_handle_special_functions(char *key, char *expression, int *expr_index);

#define MAX_EXPRESSION_LENGTH 50

// Add these new global variables
int shift_mode = 0;
int alpha_mode = 0;

int main(void) {
    char expression[MAX_EXPRESSION_LENGTH] = "";
    int expr_index = 0;

    lcd_init();
    keypad_init();

    lcd_string("Calculator ready");
    _delay_ms(2000);
    lcd_clear();

    lcd_string("Enter expression:");
    lcd_command(LCD_SET_CURSOR | 0x40);  // Move to second line

    while(1) {
        char key = keypad_scan();

        if (key != 0) {
            if (key == 'S') {
                shift_mode = !shift_mode;
                lcd_clear();
                lcd_string(shift_mode ? "Shift mode ON" : "Shift mode OFF");
                _delay_ms(1000);
                lcd_clear();
                lcd_string("Enter expression:");
                lcd_command(LCD_SET_CURSOR | 0x40);
                lcd_string(expression);
            } else if (key == 'A') {
                alpha_mode = !alpha_mode;
                lcd_clear();
                lcd_string(alpha_mode ? "Alpha mode ON" : "Alpha mode OFF");
                _delay_ms(1000);
                lcd_clear();
                lcd_string("Enter expression:");
                lcd_command(LCD_SET_CURSOR | 0x40);
                lcd_string(expression);
            } else if (key == 'C') {
                lcd_clear();
                lcd_string("Enter expression:");
                lcd_command(LCD_SET_CURSOR | 0x40);
                expr_index = 0;
                expression[expr_index] = '\0';
            } else if (key == 'B' && expr_index > 0) {
                expr_index--;
                expression[expr_index] = '\0';
                lcd_command(LCD_SET_CURSOR | 0x40);
                lcd_string("                ");  // Clear second line
                lcd_command(LCD_SET_CURSOR | 0x40);
                lcd_string(expression);
            } else if (key == '=') {
                expression[expr_index] = '\0';
                lcd_clear();
                lcd_string("Result:");
                lcd_command(LCD_SET_CURSOR | 0x40);
                evaluate_expression(expression);
                expr_index = 0;
                _delay_ms(3000);
                lcd_clear();
                lcd_string("Enter expression:");
                lcd_command(LCD_SET_CURSOR | 0x40);
            } else if (expr_index < MAX_EXPRESSION_LENGTH - 1) {
                extended_handle_special_functions(&key, expression, &expr_index);
                if (shift_mode) {
                    handle_shift_mode(&key);
                } else if (alpha_mode) {
                    handle_alpha_mode(&key);
                }
                expression[expr_index++] = key;
                lcd_char(key);
            }
            _delay_ms(300);  // Debounce
        }
    }

    return 0;
}

void handle_shift_mode(char *key) {
    // Implement shift mode functionality here
    shift_mode = 1;
}

void handle_alpha_mode(char *key) {
    // Implement alpha mode functionality here
    alpha_mode = 1;
}

float perform_exponentiation(float a, float b) {
    return pow(a, b);
}

float perform_operation_extended(float a, float b, char op) {
    if (op == '^') {
        return perform_exponentiation(a, b);
    } else {
        return perform_operation(a, b, op);
    }
}

float calculate_sin_degrees(float angle_in_degrees) {
    return sin(angle_in_degrees * M_PI / 180.0);
}

float calculate_cos_degrees(float angle_in_degrees) {
    return cos(angle_in_degrees * M_PI / 180.0);
}

float calculate_tan_degrees(float angle_in_degrees) {
    return tan(angle_in_degrees * M_PI / 180.0);
}

void display_trig_result(char *func_name, float angle, float result) {
    char display_str[32];
    char angle_str[8];
    char result_str[16];
    
    dtostrf(angle, 4, 1, angle_str);
    dtostrf(result, 8, 4, result_str);
    
    lcd_clear();
    lcd_string(func_name);
    lcd_string("(");
    lcd_string(angle_str);
    lcd_string(")");
    lcd_command(LCD_SET_CURSOR | 0x40);
    lcd_string(result_str);
    _delay_ms(2000);
}

void extended_handle_special_functions(char *key, char *expression, int *expr_index) {
    if (*key == 's' || *key == 'c' || *key == 't') {
        char buffer[16];
        float angle, result;
        
        if (*key == 's') {
            strcpy(buffer, "sin(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer) - 1;
            lcd_string(buffer);
        } else if (*key == 'c') {
            strcpy(buffer, "cos(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer) - 1;
            lcd_string(buffer);
        } else if (*key == 't') {
            strcpy(buffer, "tan(");
            strcat(expression, buffer);
            *expr_index += strlen(buffer) - 1;
            lcd_string(buffer);
        }
    } else if (*key == 'p') {
        *key = '3';
        strcat(expression, ".14159");
        *expr_index += 5;
        lcd_string("3.14159");
    } else if (*key == 'e') {
        *key = '2';
        strcat(expression, ".71828");
        *expr_index += 5;
        lcd_string("2.71828");
    }
}

void evaluate_expression(char *expression) {
    char *expr = expression;
    float result = parse_number(&expr);

    while (*expr) {
        char op = *expr++;
        if (op == 's' && strncmp(expr, "in(", 3) == 0) {
            expr += 3;
            float angle = parse_number(&expr);
            result = calculate_sin_degrees(angle);
            display_trig_result("sin", angle, result);
            expr++;  // Skip closing parenthesis
        } else if (op == 'c' && strncmp(expr, "os(", 3) == 0) {
            expr += 3;
            float angle = parse_number(&expr);
            result = calculate_cos_degrees(angle);
            display_trig_result("cos", angle, result);
            expr++;  // Skip closing parenthesis
        } else if (op == 't' && strncmp(expr, "an(", 3) == 0) {
            expr += 3;
            float angle = parse_number(&expr);
            result = calculate_tan_degrees(angle);
            display_trig_result("tan", angle, result);
            expr++;  // Skip closing parenthesis
        } else {
            float num = parse_number(&expr);
            result = perform_operation_extended(result, num, op);
        }
    }

    char result_str[16];
    dtostrf(result, 8, 2, result_str);
    lcd_string(result_str);
}

float perform_operation(float a, float b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
        case '^': return perform_exponentiation(a, b);
        default: return 0;
    }
}

float parse_number(char **expr) {
    float num = 0;
    int decimal_places = 0;
    int is_decimal = 0;

    while (**expr >= '0' && **expr <= '9' || **expr == '.') {
        if (**expr == '.') {
            is_decimal = 1;
        } else {
            if (is_decimal) {
                decimal_places++;
                num += (**expr - '0') / (float)pow(10, decimal_places);
            } else {
                num = num * 10 + (**expr - '0');
            }
        }
        (*expr)++;
    }
    return num;
}

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