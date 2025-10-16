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
int errorCount = 0;

void setup() {
  Serial.begin(115200);
  display.init();
  display.drawHappyMac();
  int status = connectWifi();
  if (status != 0) {
    display.clear();
    display.drawSadMac(std::string("Connection failed :("));
    delay(5000);
    ESP.restart();
  }
  configTime(0, 0, NPT_SERVER);
  waitForTime();
  delay(1000);
}

void loop() {
  display.clear();

  std::vector<Booking> bookings;

  try {
    bookings = getBookings(ROOM_ID);
    errorCount = 0;
  } catch (const std::exception& e) {
    Serial.println(e.what());
    errorCount += 1;
    Serial.printf("Error count: %d\n", errorCount);
    if (errorCount > MAX_ERROR_COUNT) {
      display.drawSadMac(std::string(e.what()));
      // wait longer if there are more than MAX_ERROR_COUNT errors
      delay(ERR_REFRESH_INTERVAL);
    } else {
      delay(REFRESH_INTERVAL);
    }
    // return early, keep what's on screen
    return;
  }

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

int connectWifi() {
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  int counter = 0;
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    counter += 1;
    if (counter > 60) {
      Serial.print("Failed to connect to wifi");
      return -1;
    }
  }
  return 0;
}
