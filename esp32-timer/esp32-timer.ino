hw_timer_t *timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;

void ARDUINO_ISR_ATTR onTimer() { xSemaphoreGiveFromISR(timerSemaphore, NULL); }

void setup() {
  Serial.begin(115200);

  timerSemaphore = xSemaphoreCreateBinary();
  timer = timerBegin(/* frequency = */ 1000000);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, /* alarm_value */
             1000000, /* autoreload = */ true,
             /* reload_count = */ 0);
}

void loop() {
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE) {
    Serial.println("Timer fired.");
  }
}
