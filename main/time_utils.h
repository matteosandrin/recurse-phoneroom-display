#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <string>

#include "time.h"

void waitForTime();

std::string timestampToIso8601(time_t t);

time_t iso8601ToTimestamp(const std::string& isoString);

std::string timestampToLocalHoursMins(time_t t);

time_t timegm(struct tm* tm);

#endif  // TIME_UTILS_H
