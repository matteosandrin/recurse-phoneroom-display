#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <vector>

#include "models.h"

void connectWifi();

// Fetch bookings from API for a specific room
std::vector<Booking> getBookings(int roomId);

#endif  // NETWORK_UTILS_H
