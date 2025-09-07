#include <WiFi.h>
#include <sntp.h>
#include <time.h>

// Change to your own WiFi info
const char *ssid = "dlink-ACC9";
const char *password = "cookiejar";

// https://www.ntppool.org/zone/north-america, or use a corporate server such as
// time.apple.com, time.google.com, time.windows.com
const char *ntpServer = "north-america.pool.ntp.org";
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
const char *timeZone = "PST8PDT,M3.2.0,M11.1.0";

void setup() {
  Serial.begin(115200);

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
}

void loop() {
  // Resync with NTP server once a day
  static long lastSync = millis(); // What does `static` do here?
  long millisNow = millis();
  if (millisNow - lastSync > 86400 * 1000) {
    // How do we test this without waiting for a whole day?
    lastSync = millisNow;
    Serial.println("Syncing with NTP...");
    configTzTime(timeZone, ntpServer);
  }

  sntp_sync_status_t syncStatus = sntp_get_sync_status();
  if (syncStatus == SNTP_SYNC_STATUS_COMPLETED) {
    Serial.println("Time synchronized.");
  }

  // https://cplusplus.com/reference/ctime/time/
  // https://cplusplus.com/reference/ctime/tm/
  // https://cplusplus.com/reference/ctime/localtime/
  time_t now = time(nullptr);
  struct tm *timeinfo = localtime(&now);

  // https://cplusplus.com/reference/cstdio/sprintf/
  char buffer[5]; // Why 5?
  sprintf(buffer, "%02d", timeinfo->tm_hour);
  Serial.print(buffer);
  Serial.print(":");
  sprintf(buffer, "%02d", timeinfo->tm_min);
  Serial.print(buffer);
  Serial.print(":");
  sprintf(buffer, "%02d", timeinfo->tm_sec);
  Serial.print(buffer);
  Serial.print("  ");

  const char *monthNames[12] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
                                "JUL", "AUG", "SEP", "OCT", "NOV", "DEV"};
  Serial.print(monthNames[timeinfo->tm_mon]);
  Serial.print(" ");
  sprintf(buffer, "%02d", timeinfo->tm_mday);
  Serial.print(buffer);
  Serial.print(" ");
  sprintf(buffer, "%d", timeinfo->tm_year + 1900); // Why just %d?
  Serial.print(buffer);

  Serial.println();

  delay(5000);
}