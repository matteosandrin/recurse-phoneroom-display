#include <WiFi.h>

#include "booking.h"
#include "config.h"
#include "display.h"
#include "models.h"
#include "time_utils.h"

#if __has_include("secrets.h")
#include "secrets.h"
#else
#error \
    "secrets.h not found! Please copy secrets.example.h to secrets.h and fill in your credentials."
#endif

Display display;
RoomStatus oldStatus;

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

  RoomStatus newStatus = getRoomStatus(bookings);
  printRoomStatus(newStatus);

  if (!areRoomStatusEqual(oldStatus, newStatus)) {
    Serial.println("New status, update display.");
    display.drawRoomStatus(newStatus);
    oldStatus = newStatus;
  } else {
    Serial.println("Status is the same, not updating display.");
  }

  delay(REFRESH_INTERVAL);
}

void connectWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}
