#include <avr/io.h>
#include <util/delay.h>

#define OUTPUT 1
#define HIGH 1
#define LOW 0

// Segment input pins of decoder
const int decoderPins[] = {8,9,10,11};

// Display control pins (Common Cathode)
const int displayPins[] = {7, 6, 5, 4, 3, 2, 1};

// Function to set pins as output
void pinMode(uint8_t pin, uint8_t mode) {
    if (mode == OUTPUT) {
        if (pin < 8) {
            DDRD |= (1 << pin);
        } else if (pin < 14) {
            DDRB |= (1 << (pin - 8));
        }
    }
}

// Function to control pin states
void digitalWrite(uint8_t pin, uint8_t value) {
    if (value == HIGH) {
        if (pin < 8) {
            PORTD |= (1 << pin);
        } else if (pin < 14) {
            PORTB |= (1 << (pin - 8));
        }
    } else {
        if (pin < 8) {
            PORTD &= ~(1 << pin);
        } else if (pin < 14) {
            PORTB &= ~(1 << (pin - 8));
        }
    }
}

// Function to set the BCD value for the 7447 decoder
void setBCD(int digit, const int* decoderPins) {
    // Fixed syntax error in function declaration
    
    // For BCD encoding, we just need to set the bits directly
    // A more efficient approach than switch-case
    digitalWrite(decoderPins[0], digit & 0x01);       // A (LSB)
    digitalWrite(decoderPins[1], (digit >> 1) & 0x01); // B
    digitalWrite(decoderPins[2], (digit >> 2) & 0x01); // C
    digitalWrite(decoderPins[3], (digit >> 3) & 0x01); // D (MSB)
}

int main() {
    // Initialize pins as OUTPUT
    for (int i = 0; i < 4; i++) {
        pinMode(decoderPins[i], OUTPUT);  // Set decoder pins
    }
    for (int i = 0; i < 6; i++) {
        pinMode(displayPins[i], OUTPUT);  // Set display pins
    }

    int hours = 12;    // Example initial time
    int minutes = 30;
    int seconds = 45;
    
    while (1) {
        // Multiplex through all 6 displays for approximately 1 second
        unsigned long start_time = 0;
        while (start_time < 1000) { // Run for about 1 second
            // Multiplex through all 6 displays
            for (int display = 0; display < 6; display++) {
                // First turn off all displays to prevent ghosting
                for (int i = 0; i < 6; i++) {
                    digitalWrite(displayPins[i], LOW);
                }
                
                // Set BCD value for the current digit before enabling the display
                if (display == 0) setBCD(hours / 10, decoderPins);       // Tens place of hours
                else if (display == 1) setBCD(hours % 10, decoderPins);  // Units place of hours
                else if (display == 2) setBCD(minutes / 10, decoderPins); // Tens place of minutes
                else if (display == 3) setBCD(minutes % 10, decoderPins); // Units place of minutes
                else if (display == 4) setBCD(seconds / 10, decoderPins); // Tens place of seconds
                else if (display == 5) setBCD(seconds % 10, decoderPins); // Units place of seconds
                
                // Enable only the current display
                digitalWrite(displayPins[display], HIGH);
                
                // Short delay for this digit
                _delay_ms(2);  // Adjust this value for optimal refresh rate
                
                start_time += 2; // Approximate time tracking
            }
        }
        
        // Update time after approximately 1 second
        seconds++;
        if (seconds == 60) {
            seconds = 0;
            minutes++;
        }
        if (minutes == 60) {
            minutes = 0;
            hours++;
        }
        if (hours == 24) {
            hours = 0;
        }
    }

    return 0;
}
