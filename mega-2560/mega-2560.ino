// Test sketch for displaying time and date with an Arduino Mega 2560

#include <RGBmatrixPanel.h>

#include <stdlib.h>
#include <string.h>

// RGB pins: 24, 25, 26, 27, 28, 29

#define CLK 11
#define OE 9
#define LAT 10
#define A A0
#define B A1
#define C A2
#define D A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 64);

void setup() {
  matrix.begin();
  delay(500);

  matrix.setRotation(2);
  matrix.setTextWrap(false);

  matrix.setTextSize(2);
  matrix.setCursor(3, 3);
  matrix.setTextColor(matrix.Color333(7, 0, 0));
  matrix.print("20:12");

  matrix.setTextSize(1);
  matrix.setTextColor(matrix.Color333(0, 7, 0));
  matrix.setCursor(3, 21);
  matrix.print("AUG");
  matrix.setTextColor(matrix.Color333(7, 0, 7));
  matrix.setCursor(24, 21);
  matrix.print("31");
  matrix.setTextColor(matrix.Color333(7, 7, 0));
  matrix.setCursor(38, 21);
  matrix.print("2025");
}

void loop() {}
