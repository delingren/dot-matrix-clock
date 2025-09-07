#include <Adafruit_Protomatter.h>

// 'Safe' pins, not overlapping any peripherals:
// GPIO.out: 4, 12, 13, 14, 15, 21, 27, GPIO.out1: 32, 33
// Peripheral-overlapping pins, sorted from 'most expendible':
// 16, 17 (RX, TX)
// 25, 26 (A0, A1)
// 18, 5, 9 (MOSI, SCK, MISO)
// 22, 23 (SCL, SDA)
uint8_t rgbPins[] = {25, 26, 27, 14, 12, 13};
uint8_t addrPins[] = {23, 19, 5, 17};
uint8_t clockPin = 16;
uint8_t latchPin = 4;
uint8_t oePin = 15;

Adafruit_Protomatter
    matrix(64,          // Width of matrix (or matrix chain) in pixels
           1,           // Bit depth, 1-6
           1, rgbPins,  // # of matrix chains, array of 6 RGB pins for each
           4, addrPins, // # of address pins (height is inferred), array of pins
           clockPin, latchPin, oePin, // Other matrix control pins
           false); // No double-buffering here (see "doublebuffer" example)

void setup(void) {
  Serial.begin(9600);

  // Initialize matrix...
  ProtomatterStatus status = matrix.begin();
  Serial.print("Protomatter begin() status: ");
  Serial.println((int)status);
  if (status != PROTOMATTER_OK) {
    for (;;)
      ;
  }

  matrix.setRotation(2);
  matrix.setTextWrap(false);

  matrix.setTextSize(2);
  matrix.setCursor(3, 3);
  matrix.setTextColor(matrix.color565(255, 0, 0));
  matrix.print("20:12");

  matrix.setTextSize(1);
  matrix.setTextColor(matrix.color565(0, 255, 0));
  matrix.setCursor(3, 21);
  matrix.print("AUG");
  matrix.setTextColor(matrix.color565(255, 0, 255));
  matrix.setCursor(24, 21);
  matrix.print("31");
  matrix.setTextColor(matrix.color565(255, 255, 0));
  matrix.setCursor(38, 21);
  matrix.print("2025");

  matrix.show(); // Copy data to matrix buffers
}

void loop(void) {
  Serial.print("Refresh FPS = ~");
  Serial.println(matrix.getFrameCount());
  delay(1000);
}
