// Test sketch for displaying time and date with an ESP32 Wroom

// Pin assignment:

// #define R1_PIN_DEFAULT  25
// #define G1_PIN_DEFAULT  26
// #define B1_PIN_DEFAULT  27
// #define R2_PIN_DEFAULT  14
// #define G2_PIN_DEFAULT  12
// #define B2_PIN_DEFAULT  13

// #define A_PIN_DEFAULT   23
// #define B_PIN_DEFAULT   19
// #define C_PIN_DEFAULT   5
// #define D_PIN_DEFAULT   17
            
// #define LAT_PIN_DEFAULT 4
// #define OE_PIN_DEFAULT  15
// #define CLK_PIN_DEFAULT 16

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
 
void setup() {
  HUB75_I2S_CFG mxconfig(64, 32, 1);
  mxconfig.driver = HUB75_I2S_CFG::FM6124;

  MatrixPanel_I2S_DMA matrix(mxconfig);
  matrix.begin();
  delay(500);

  matrix.setRotation(2);
  matrix.setTextWrap(false);

  matrix.setTextSize(2);
  matrix.setCursor(3, 3);
  matrix.setTextColor(matrix.color444(15, 0, 0));
  matrix.print("20:12");

  matrix.setTextSize(1);
  matrix.setTextColor(matrix.color444(0, 15, 0));
  matrix.setCursor(3, 21);
  matrix.print("AUG");
  matrix.setTextColor(matrix.color444(15, 0, 15));
  matrix.setCursor(24, 21);
  matrix.print("31");
  matrix.setTextColor(matrix.color444(15, 15, 0));
  matrix.setCursor(38, 21);
  matrix.print("2025");
}

void loop() {}
