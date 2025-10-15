#ifndef BOOKING_H
#define BOOKING_H

#include <ArduinoJson.h>

#include <vector>

#include "models.h"

Booking jsonToBooking(const JsonObject& object);

RoomStatus getRoomStatus(const std::vector<Booking>& bookings);

void printBooking(const Booking& booking);
void printRoomStatus(const RoomStatus& status);

#endif  // BOOKING_H
