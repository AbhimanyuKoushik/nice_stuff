// Segment control pins (A-G)
const int segmentPins[] = {A0, A1, A2, A3, A4, A5, 8};

// Display control pins (Common Cathode)
const int displayPins[] = {7, 6, 5, 4, 3, 2, 1}; // Using A0 and A1 as additional digital pins

// Digit patterns for numbers 0-9 (Common Cathode)
const byte digitPatterns[10] = {
  B00111111, // 0
  B00000110, // 1
  B01011011, // 2
  B01001111, // 3
  B01100110, // 4
  B01101101, // 5
  B01111101, // 6
  B00000111, // 7
  B01111111, // 8
  B01101111  // 9
};

// Initial time (10:10:00)
int hours = 23;
int minutes = 59;
int seconds = 50;

// Timing variables
unsigned long previousMillis = 0;
const long secondInterval = 992; // 1 second

void setup() {
  // Set segment pins as outputs
  for (int i = 0; i < 7; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }

  // Set display control pins as outputs
  for (int i = 0; i < 6; i++) {
    pinMode(displayPins[i], OUTPUT);
    digitalWrite(displayPins[i], HIGH); // Turn off all displays initially
  }
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= secondInterval) {
    previousMillis = currentMillis;
    incrementTime();
  }
  displayTime(hours, minutes, seconds);
}

void incrementTime() {
  seconds++;
  if (seconds >= 60) {
    seconds = 0;
    minutes++;
    if (minutes >= 60) {
      minutes = 0;
      hours++;
      if (hours >= 24) {
        hours = 0;
      }
    }
  }
}

void displayTime(int hours, int minutes, int seconds) {
  int digits[6] = {hours / 10, hours % 10, minutes / 10, minutes % 10, seconds / 10, seconds % 10};

  // Display hours, minutes, and seconds on separate displays
  for (int i = 0; i < 6; i++) {
    showDigit(digits[i], i);
    delay(3); // Adjust delay for brightness
  }
}

void showDigit(int number, int display) {
  // Turn off all displays
  for (int i = 0; i < 6; i++) {
    digitalWrite(displayPins[i], HIGH);
  }

  // Display the number on the selected display
  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentPins[i], bitRead(digitPatterns[number], i));
  }

  // Turn on the selected display
  digitalWrite(displayPins[display], LOW);

  // Short delay for persistence of vision
  delay(5);
}