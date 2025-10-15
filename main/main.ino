#include "booking.h"
#include "config.h"
#include "display.h"
#include "models.h"
#include "network_utils.h"
#include "time_utils.h"

#if __has_include("secrets.h")
#include "secrets.h"
#else
#error \
    "secrets.h not found! Please copy secrets.example.h to secrets.h and fill in your credentials."
#endif

// Global display object
Display display;

void setup() {
  Serial.begin(115200);
  display.init();
  display.drawWifiDebug();
  connectWifi();
  configTime(0, 0, NPT_SERVER);
  waitForTime();
  delay(1000);
}

void loop() {
  display.clear();

  std::vector<Booking> bookings = getBookings(ROOM_ID);

  Serial.println("Bookings:");
  for (const Booking& booking : bookings) {
    printBooking(booking);
  }

  RoomStatus status = getRoomStatus(bookings);
  printRoomStatus(status);
  display.drawRoomStatus(status);
  delay(REFRESH_INTERVAL);
}
