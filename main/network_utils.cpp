#include "network_utils.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>

#include "booking.h"
#include "config.h"
#include "secrets.h"
#include "time.h"
#include "time_utils.h"

void connectWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

std::vector<Booking> getBookings(int roomId) {
  WiFiClientSecure clientSecure;
  WiFiClient client;
  HTTPClient http;

  time_t now = time(nullptr);
  std::string isoTime = timestampToIso8601(now);

  std::string url = API_URL;
  url += "?end_time=";
  url += isoTime;
  url += "&end_time_op=%3E";  // > operator
  url += "&room_id=";
  url += std::to_string(ROOM_ID);
  url += "&limit=2";

  Serial.println(url.c_str());
  std::string cookie = std::string("auth_token=") + AUTH_TOKEN;
  http.addHeader("Cookie", cookie.c_str());

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
    return std::vector<Booking>();
  }

  DynamicJsonDocument doc(2048);
  DeserializationError jsonErr = deserializeJson(doc, http.getStream());
  http.end();

  if (jsonErr) {
    Serial.println(jsonErr.c_str());
    return std::vector<Booking>();
  }

  serializeJson(doc, Serial);
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
