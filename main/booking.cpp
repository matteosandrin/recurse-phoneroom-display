#include "booking.h"

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>

#include "config.h"
#include "secrets.h"
#include "time.h"
#include "time_utils.h"

Booking jsonToBooking(const JsonObject& object) {
  return Booking{.id = object["id"],
                 .user_id = object["user_id"],
                 .user_name = object["user_name"],
                 .room_id = object["room_id"],
                 .room_name = object["room_name"],
                 .start_time = iso8601ToTimestamp(object["start_time"]),
                 .end_time = iso8601ToTimestamp(object["end_time"]),
                 .notes = object["notes"]};
}

std::vector<Booking> getBookings(int roomId) {
  WiFiClientSecure clientSecure;
  WiFiClient client;
  HTTPClient http;

  time_t now = time(nullptr);
  std::string nowIsoTime = timestampToIso8601(now);

  time_t nextMidnight = getNextMidnight(now);
  std::string nextMidnightIsoTime = timestampToIso8601(nextMidnight);

  std::string url = API_URL;
  // end_time is after now
  url += "?end_time=";
  url += nowIsoTime;
  url += "&end_time_op=%3E";  // > operator
  // start_time is before midnight today
  url += "&start_time=";
  url += nextMidnightIsoTime;
  url += "&start_time_op=%3C";  // < operator
  url += "&room_id=";
  url += std::to_string(ROOM_ID);
  url += "&limit=2";

  Serial.println(url.c_str());
  #ifdef AUTH_TOKEN:
    std::string cookie = std::string("auth_token=") + AUTH_TOKEN;
    http.addHeader("Cookie", cookie.c_str());
  #endif
  #ifdef API_KEY:
    std::string authorization = std::string("Bearer ") + API_KEY;
    http.addHeader("Authorization", authorization.c_str());
  #endif

  // Use HTTPS or HTTP based on the URL scheme
  if (url.find("https://") == 0) {
    clientSecure.setInsecure();  // Skip certificate validation
    http.begin(clientSecure, url.c_str());
  } else {
    http.begin(client, url.c_str());
  }

  int code = http.GET();
  if (code != HTTP_CODE_OK) {
    Serial.println("Error while fetching bookings:");
    Serial.println(http.errorToString(code));
    throw std::runtime_error("Exception occurred during HTTP request");
  }

  DynamicJsonDocument doc(2048);
  DeserializationError jsonErr = deserializeJson(doc, http.getStream());
  http.end();

  if (jsonErr) {
    Serial.println(jsonErr.c_str());
    throw std::runtime_error("Exception occurred while deserializing JSON");
  }

  serializeJson(doc, Serial);
  Serial.println();
  JsonArray arr = doc.as<JsonArray>();

  if (arr.size() == 0) {
    return std::vector<Booking>();
  }

  std::vector<Booking> bookings;
  for (JsonObject item : arr) {
    bookings.push_back(jsonToBooking(item));
  }

  return bookings;
}

RoomStatus getRoomStatus(const std::vector<Booking>& bookings) {
  const DisplayBooking availableEodBooking =
      DisplayBooking{.title = "AVAILABLE", .subtitle = "until end of day"};

  if (bookings.empty()) {
    return RoomStatus{.now = availableEodBooking, .next = availableEodBooking};
  }

  time_t now = time(nullptr);
  Booking firstBooking = bookings[0];
  bool hasNow = firstBooking.start_time <= now;

  std::string firstTitle = (firstBooking.notes.length() > 0)
                               ? firstBooking.notes
                               : firstBooking.user_name;
  std::string firstSubtitle =
      std::string("until ") + timestampToLocalHoursMins(firstBooking.end_time);

  if (!hasNow) {
    // The room is currently available
    // The first booking becomes the next booking
    return RoomStatus{
        .now = DisplayBooking{.title = std::string("AVAILABLE"),
                              .subtitle = std::string("until ") +
                                          timestampToLocalHoursMins(
                                              firstBooking.start_time)},
        .next = DisplayBooking{
            .title = firstTitle,
            .subtitle = timestampToLocalHoursMins(firstBooking.start_time) +
                        std::string(" - ") +
                        timestampToLocalHoursMins(firstBooking.end_time)}};
  }

  // The room is currently occupied
  RoomStatus status = RoomStatus{
      .now = DisplayBooking{.title = firstTitle, .subtitle = firstSubtitle}};

  bool hasNext = bookings.size() > 1;
  if (hasNext) {
    // The room is currently occupied, and there is a next booking
    Booking secondBooking = bookings[1];
    std::string secondTitle = (secondBooking.notes.length() > 0)
                                  ? secondBooking.notes
                                  : secondBooking.user_name;
    std::string secondSubtitle =
        timestampToLocalHoursMins(secondBooking.start_time) +
        std::string(" - ") + timestampToLocalHoursMins(secondBooking.end_time);
    status.next = DisplayBooking{.title = secondTitle,
                                 .subtitle = secondSubtitle};
  } else {
    status.next = availableEodBooking;
  }

  return status;
}

bool areRoomStatusEqual(const RoomStatus& statusA, const RoomStatus& statusB) {
  if (statusA.now.title != statusB.now.title ||
      statusA.now.subtitle != statusB.now.subtitle) {
    return false;
  }

  if (statusA.next.title != statusB.next.title ||
      statusA.next.subtitle != statusB.next.subtitle) {
    return false;
  }

  return true;
}

void printBooking(const Booking& booking) {
  Serial.println("Booking {");
  Serial.print("  id: ");
  Serial.println(booking.id.c_str());
  Serial.print("  user_id: ");
  Serial.println(booking.user_id.c_str());
  Serial.print("  user_name: ");
  Serial.println(booking.user_name.c_str());
  Serial.print("  room_id: ");
  Serial.println(booking.room_id.c_str());
  Serial.print("  room_name: ");
  Serial.println(booking.room_name.c_str());
  Serial.print("  start_time: ");
  Serial.print(booking.start_time);
  Serial.print(" (");
  Serial.print(timestampToIso8601(booking.start_time).c_str());
  Serial.println(")");
  Serial.print("  end_time: ");
  Serial.print(booking.end_time);
  Serial.print(" (");
  Serial.print(timestampToIso8601(booking.end_time).c_str());
  Serial.println(")");
  Serial.print("  notes: ");
  Serial.println(booking.notes.c_str());
  Serial.println("}");
}

void printRoomStatus(const RoomStatus& status) {
  Serial.println("RoomStatus {");

  Serial.println("  now: {");
  Serial.print("    title: ");
  Serial.println(status.now.title.c_str());
  Serial.print("    subtitle: ");
  Serial.println(status.now.subtitle.c_str());
  Serial.println("  }");

  Serial.println("  next: {");
  Serial.print("    title: ");
  Serial.println(status.next.title.c_str());
  Serial.print("    subtitle: ");
  Serial.println(status.next.subtitle.c_str());
  Serial.println("  }");

  Serial.println("}");
}
