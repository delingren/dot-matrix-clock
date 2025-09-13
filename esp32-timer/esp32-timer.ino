hw_timer_t *timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;

// This is the interrupt handler that runs every time the timer fires. Keep
// it really short and fast! The rest of the logic should be in loop().
void ARDUINO_ISR_ATTR onTimer() { xSemaphoreGiveFromISR(timerSemaphore, NULL); }

void setup() {
  Serial.begin(115200);

  timerSemaphore = xSemaphoreCreateBinary();
  // Create a clock at 1Mhz
  timer = timerBegin(/* frequency = */ 1000000);
  timerAttachInterrupt(timer, &onTimer);
  // Set an alarm to fire every 1000000 ticks. That's exactly one second.
  // Also tell it to autoreload (set another alarm when it fires) infinitely.
  timerAlarm(timer, /* alarm_value */
             1000000, /* autoreload = */ true,
             /* reload_count = */ 0);
}

void loop() {
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE) {
    Serial.println("Timer fired.");
    // Do something every second.
  }
  // Otherwise, there is nothing to do in this example.
}
