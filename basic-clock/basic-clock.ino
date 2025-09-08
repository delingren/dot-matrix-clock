#include <Adafruit_Protomatter.h>
#include <WiFi.h>
#include <sntp.h>
#include <time.h>

/**
 * Dot matrix panel setup
 */
uint8_t rgbPins[] = {25, 26, 27, 14, 12, 13};
uint8_t addrPins[] = {23, 19, 5, 17};
uint8_t clockPin = 16;
uint8_t latchPin = 4;
uint8_t oePin = 15;

Adafruit_Protomatter
    matrix(64,          // Width of matrix (or matrix chain) in pixels
           4,           // Bit depth, 1-6
           1, rgbPins,  // # of matrix chains, array of 6 RGB pins for each
           4, addrPins, // # of address pins (height is inferred), array of pins
           clockPin, latchPin, oePin, // Other matrix control pins
           false); // No double-buffering here (see "doublebuffer" example)

/**
 * WiFi setup
 */
#include "wifi_info.h"

/**
 * NTP server and timezone setup
 */
const char *ntpServer = "north-america.pool.ntp.org";
const char *timeZone = "PST8PDT,M3.2.0,M11.1.0";

/**
 * Timer setup
 */
hw_timer_t *timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
void ARDUINO_ISR_ATTR onTimer() { xSemaphoreGiveFromISR(timerSemaphore, NULL); }

void setup() {
  Serial.begin(115200);

  ProtomatterStatus status = matrix.begin();
  Serial.print("Initializing dot matrix panel. Status: ");
  Serial.println((int)status);
  if (status != PROTOMATTER_OK) {
    while (true)
      ;
  }
  matrix.setRotation(2);

  // TODO: display the following info on the panel.

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");

  // Wait for the connection to be established.
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.print("\nWiFi connected: ");
  Serial.println(WiFi.localIP());

  // Sync time with NTP server
  configTzTime(timeZone, ntpServer);

  // Wait for the sync to be completed.
  Serial.println("Waiting for NTP time synchronization...");
  while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.print("\nTime synchronized: ");
  Serial.println(time(nullptr));

  // Set up timer to trigger an update every second
  timerSemaphore = xSemaphoreCreateBinary();
  // Set timer resolution to 1Mhz; trigger the timer ervery 1M cycles (1s).
  timer = timerBegin(/* frequency = */ 1000000);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, /* alarm_value = */
             1000000, /* autoreload = */ true,
             /* reload_count = */ 0);
}

void loop() {
  // We only update once per second.
  if (xSemaphoreTake(timerSemaphore, 0) != pdTRUE) {
    return;
  }

  time_t now = time(nullptr);
  struct tm *timeinfo = localtime(&now);

  // Update time display every second
  updateTimeDisplay(timeinfo);

  // Update the date display when the day changes and on the first run.
  static int day = 0;
  if (timeinfo->tm_mday != day) {
    updateDateDisplay(timeinfo);
    if (day != 0) {
      // Resync with NTP server once a day
      configTzTime(timeZone, ntpServer);
    }
    day = timeinfo->tm_mday;
  }

  sntp_sync_status_t syncStatus = sntp_get_sync_status();
  if (syncStatus == SNTP_SYNC_STATUS_COMPLETED) {
    // TODO: display a notification on the panel
    Serial.println("Time synchronized.");
  }

  matrix.show();
}

void updateTimeDisplay(tm *timeinfo) {
  matrix.setTextSize(2);
  matrix.setTextColor(color444(15, 15, 15), color444(0, 0, 0));

  // We want to colon to be a little narrow. So print it first and let hour
  // and minute overlap with its left and right a little, which is blank anyway.
  static bool colon = true;
  matrix.setCursor(27, 3);
  matrix.print(colon ? ":" : " ");
  colon = !colon;

  char buffer[3];
  sprintf(buffer, "%02d", timeinfo->tm_hour, timeinfo->tm_hour);
  matrix.setCursor(5, 3);
  matrix.print(buffer);

  sprintf(buffer, "%02d", timeinfo->tm_hour, timeinfo->tm_min);
  matrix.setCursor(39, 3);
  matrix.print(buffer);
}

void updateDateDisplay(tm *timeinfo) {
  char buffer[5];

  matrix.setTextSize(1);

  const char *monthNames[12] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
                                "JUL", "AUG", "SEP", "OCT", "NOV", "DEV"};
  matrix.setTextColor(color444(8, 8, 0), color444(0, 0, 0));
  matrix.setCursor(3, 21);
  matrix.print(monthNames[timeinfo->tm_mon]);

  matrix.setTextColor(color444(0, 8, 8), color444(0, 0, 0));
  matrix.setCursor(24, 21);
  sprintf(buffer, "%02d", timeinfo->tm_mday);
  matrix.print(buffer);

  matrix.setTextColor(color444(8, 0, 8), color444(0, 0, 0));
  matrix.setCursor(38, 21);
  sprintf(buffer, "%d", timeinfo->tm_year + 1900);
  matrix.print(buffer);
}

inline uint16_t color444(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0x0F) << 12) | ((g & 0x0F) << 7) | ((b & 0x0F) << 1);
}