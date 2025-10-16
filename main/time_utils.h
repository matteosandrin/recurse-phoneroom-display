#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <string>

#include "time.h"

void setupTime(char *timezone);

void _waitForTime();

std::string timestampToIso8601(time_t t);

time_t iso8601ToTimestamp(const std::string& isoString);

std::string timestampToLocalHoursMins(time_t t);

time_t getNextMidnight(time_t t);

time_t _timegm(struct tm* tm);

#endif  // TIME_UTILS_H
