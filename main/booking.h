#ifndef BOOKING_H
#define BOOKING_H

#include <ArduinoJson.h>

#include <vector>

#include "models.h"

Booking jsonToBooking(const JsonObject& object);

std::vector<Booking> getBookings(int roomId);

RoomStatus getRoomStatus(const std::vector<Booking>& bookings);

bool areRoomStatusEqual(const RoomStatus& statusA, const RoomStatus& statusB);

void printBooking(const Booking& booking);
void printRoomStatus(const RoomStatus& status);

#endif  // BOOKING_H
