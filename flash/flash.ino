#include <Adafruit_Protomatter.h>

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

hw_timer_t *timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;

void ARDUINO_ISR_ATTR onTimer() { xSemaphoreGiveFromISR(timerSemaphore, NULL); }

void setup() {
  Serial.begin(115200);

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

  matrix.setTextSize(2);

  // Set up timer to trigger flashing
  timerSemaphore = xSemaphoreCreateBinary();
  timer = timerBegin(/* frequency = */ 1000000);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, /* alarm_value */
             1000000, /* autoreload = */ true,
             /* reload_count = */ 0);
}

void loop() {
  static bool colon = true;

  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE) {
    Serial.println("Flashing...");
    matrix.setCursor(27, 3);
    matrix.setTextColor(colon ? matrix.color565(255, 0, 0)
                              : matrix.color565(0, 0, 0));
    matrix.print(":");
    matrix.show();
    colon = !colon;
  }
}
