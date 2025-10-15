#include "time_utils.h"

#include <Arduino.h>

#include "config.h"

void waitForTime() {
  // Wait until SNTP sets a sane epoch (e.g., > Jan 1 2019)
  time_t now = time(nullptr);
  int retries = 0;
  while (now < 1546300800 && retries < 60) {  // 2019-01-01
    delay(500);
    now = time(nullptr);
    retries++;
  }
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
  return mktime(&tm);
}

std::string timestampToLocalHoursMins(time_t t) {
  time_t local_t = t + (TZ_OFFSET);
  struct tm* tm = gmtime(&local_t);
  char buf[16];
  strftime(buf, sizeof(buf), "%I:%M %P", tm);
  return std::string(buf);
}
