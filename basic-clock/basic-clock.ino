#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <WiFi.h>
#include <sntp.h>
#include <time.h>

/**
 * WiFi and NTP setup
 */
const char *ssid = "dlink-ACC9";
const char *password = "cookiejar";

const char *ntpServer = "north-america.pool.ntp.org";
const char *timeZone = "PST8PDT,M3.2.0,M11.1.0";

/**
 * Timer setup for flashing colon
 */
hw_timer_t *timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;

MatrixPanel_I2S_DMA *matrix;

void ARDUINO_ISR_ATTR onTimer() { xSemaphoreGiveFromISR(timerSemaphore, NULL); }

void setup() {
  Serial.begin(115200);

  HUB75_I2S_CFG config(64, 32, 1);
  config.driver = HUB75_I2S_CFG::FM6124;
  matrix = new MatrixPanel_I2S_DMA(config);
  matrix->begin();
  matrix->setRotation(2);
  // delay(500);

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");

  // Wait for the connection to be established.
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");

  // Sync time with NTP server
  configTzTime(timeZone, ntpServer);

  // Wait for the sync to be completed.
  Serial.println("Waiting for NTP time synchronization...");
  while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nTime synchronized.");

  // Set up timer to trigger flashing
  timerSemaphore = xSemaphoreCreateBinary();
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

  // Update time display every second
  updateTimeDisplay(timeinfo);
}

void updateDateDisplay(tm *timeinfo) {
  char buffer[5];

  matrix->setTextSize(1);

  const char *monthNames[12] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
                                "JUL", "AUG", "SEP", "OCT", "NOV", "DEV"};
  matrix->setTextColor(matrix->color444(0, 15, 0), matrix->color444(0, 0, 0));
  matrix->setCursor(3, 21);
  matrix->print(monthNames[timeinfo->tm_mon]);

  matrix->setTextColor(matrix->color444(15, 0, 15), matrix->color444(0, 0, 0));
  matrix->setCursor(24, 21);
  sprintf(buffer, "%02d", timeinfo->tm_mday);
  matrix->print(buffer);

  matrix->setTextColor(matrix->color444(15, 15, 0), matrix->color444(0, 0, 0));
  matrix->setCursor(38, 21);
  sprintf(buffer, "%d", timeinfo->tm_year + 1900);
  matrix->print(buffer);
}

void updateTimeDisplay(tm *timeinfo) {
  static bool colon = true;
  char buffer[6];

  sprintf(buffer, colon ? "%02d:%02d" : "%02d %02d", timeinfo->tm_hour,
          timeinfo->tm_min);
  matrix->setTextSize(2);
  matrix->setCursor(3, 3);
  matrix->setTextColor(matrix->color444(15, 0, 0), matrix->color444(0, 0, 0));
  matrix->print(buffer);
  colon = !colon;
}