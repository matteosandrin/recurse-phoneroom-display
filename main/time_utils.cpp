#include "time_utils.h"

#include <Arduino.h>

#include "config.h"

void setupTime() {
  setenv("TZ", TIMEZONE, 1);
  tzset();
  configTzTime(TIMEZONE, NTP_SERVER_1, NTP_SERVER_2);
  if (!_waitForTime()) {
    Serial.println("Time sync failed");
    return;
  }
  Serial.println("Time synced!");
}

bool _waitForTime() {
  struct tm tm;
  bool status = getLocalTime(&tm, 15000);
  char buf[32];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S %Z", &tm);
  Serial.println(buf);
  return status;
}

std::string timestampToIso8601(time_t t) {
  struct tm tm;
  gmtime_r(&t, &tm);  // UTC
  char buf[21];       // "YYYY-MM-DDTHH:MM:SSZ" = 20 + NUL
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &tm);
  return std::string(buf);
}

time_t iso8601ToTimestamp(const std::string& isoString) {
  struct tm tm;
  strptime(isoString.c_str(), "%Y-%m-%dT%H:%M:%S", &tm);
  return _timegm(&tm);
}

std::string timestampToLocalHoursMins(time_t t) {
  setenv("TZ", TIMEZONE, 1);
  tzset();
  struct tm tm;
  localtime_r(&t, &tm);
  char buf[16];
  strftime(buf, sizeof(buf), "%I:%M %P", &tm);
  return std::string(buf);
}

time_t getNextMidnight(time_t t) {
  setenv("TZ", TIMEZONE, 1);
  tzset();
  struct tm tm;
  localtime_r(&t, &tm);
  tm.tm_hour = 0;
  tm.tm_min = 0;
  tm.tm_sec = 0;
  tm.tm_mday += 1;
  return mktime(&tm);
}

// this function will always operate on UTC timezone
time_t _timegm(struct tm* tm) {
  char* original_tz = getenv("TZ");

  // Set the timezone to UTC for the mktime calculation
  setenv("TZ", "UTC", 1);
  tzset();

  time_t t = mktime(tm);

  // Restore the original timezone setting
  if (original_tz) {
    setenv("TZ", original_tz, 1);
  } else {
    unsetenv("TZ");
  }
  tzset();

  return t;
}