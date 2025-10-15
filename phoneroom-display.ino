#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>

#include <string>
#include <vector>

#include "HT_DEPG0290BxS800FxX_BW.h"
#include "time.h"

#define DIRECTION ANGLE_0_DEGREE
#define SCREEN_WIDTH 296
#define SCREEN_HEIGHT 128
#define WIFI_SSID "Recurse Center"
#define WIFI_PASSWORD "nevergraduate!"
#define NPT_SERVER "time.google.com"
#define API_URL "https://phoneroom.recurse.com/api/bookings"
#define AUTH_TOKEN \
  "4ac2768024b9c36a08d5376dab32d0e532e40db372db6a8a85a5bb6d27710c5c"
#define ROOM_ID 2
#define ROOM_NAME "Lovelace"
// TODO(msandrin) handle daylight savings in timezone
#define TZ_OFFSET -14400  // -04:00

struct Booking {
  std::string id;
  std::string user_id;
  std::string user_name;
  std::string room_id;
  std::string room_name;
  long start_time;
  long end_time;
  std::string notes;
};

struct DisplayBooking {
  std::string title;
  std::string subtitle;
};

struct RoomStatus {
  bool hasNow;
  bool hasNext;
  DisplayBooking now;
  DisplayBooking next;
};

// rst, dc, cs, busy, sck, mosi, miso, frequency
DEPG0290BxS800FxX_BW display(5, 4, 3, 6, 2, 1, -1, 6000000);

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
  Serial.print("  hasNow: ");
  Serial.println(status.hasNow ? "true" : "false");
  Serial.print("  hasNext: ");
  Serial.println(status.hasNext ? "true" : "false");

  if (status.hasNow) {
    Serial.println("  now: {");
    Serial.print("    title: ");
    Serial.println(status.now.title.c_str());
    Serial.print("    subtitle: ");
    Serial.println(status.now.subtitle.c_str());
    Serial.println("  }");
  }

  if (status.hasNext) {
    Serial.println("  next: {");
    Serial.print("    title: ");
    Serial.println(status.next.title.c_str());
    Serial.print("    subtitle: ");
    Serial.println(status.next.subtitle.c_str());
    Serial.println("  }");
  }

  Serial.println("}");
}

void setup() {
  Serial.begin(115200);
  // Initialize eink display
  VextON();
  delay(100);
  display.init();
  display.screenRotate(DIRECTION);
  display.clear();
  display.clear();
  delay(1000);
  // Connect to wifi
  display.drawString(0, 0, "init >>> ");
  Serial.print("Connecting to ");
  display.drawString(0, 20, "Connecting to ... ");
  Serial.println(WIFI_SSID);
  display.drawString(100, 20, WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display.drawString(i, 40, ".");
    i = i + 10;
  }
  Serial.println("");
  Serial.println("WiFi connected");
  display.drawString(0, 60, "WiFi connected");
  Serial.println("IP address: ");
  display.drawString(0, 90, "IP address: ");
  display.drawString(60, 90, WiFi.localIP().toString().c_str());

  Serial.println(WiFi.localIP());
  Serial.println("");
  Serial.println("WiFi Connected!");
  display.display();

  // config ntp time server
  configTime(0, 0, NPT_SERVER);
  waitForTime();

  delay(1000);
}

void VextON(void) {
  pinMode(18, OUTPUT);
  digitalWrite(18, HIGH);
}

void VextOFF(void)  // Vext default OFF
{
  pinMode(18, OUTPUT);
  digitalWrite(18, LOW);
}

void waitForTime() {
  // Wait until SNTP sets a sane epoch (e.g., > Jan 1 2019)
  time_t now = time(nullptr);
  int retries = 0;
  while (now < 1546300800 && retries < 60) {  // 2019-01-01
    delay(500);
    now = time(nullptr);
    retries++;
  }
}

void drawCurrentTime() {
  time_t now = time(nullptr);
  std::string isoTime = timestampToIso8601(now);
  Serial.println(isoTime.c_str());
  display.setFont(ArialMT_Plain_24);
  display.drawString(10, 10, isoTime.c_str());
}

std::string timestampToIso8601(time_t t) {
  struct tm tm;
  gmtime_r(&t, &tm);  // UTC
  char buf[21];       // "YYYY-MM-DDTHH:MM:SSZ" = 20 + NUL
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &tm);
  return std::string(buf);
}

time_t iso8601ToTimestamp(const std::string& isoString) {
  struct tm tm;
  strptime(isoString.c_str(), "%Y-%m-%dT%H:%M:%S", &tm);
  return mktime(&tm);
}

std::string timestampToLocalHoursMins(time_t t) {
  time_t local_t = t + (TZ_OFFSET);
  struct tm* tm = gmtime(&local_t);
  char buf[16];
  strftime(buf, sizeof(buf), "%I:%M %P", tm);
  return std::string(buf);
}

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

RoomStatus getRoomStatus(const std::vector<Booking>& bookings) {
  if (bookings.empty()) {
    return RoomStatus{.hasNow = false, .hasNext = false};
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
    // the room is currently vacant
    // the first booking becomes the next booking
    return RoomStatus{
        .hasNow = true,
        .hasNext = true,
        .now = DisplayBooking{.title = std::string("VACANT"),
                              .subtitle = std::string("until ") +
                                          timestampToLocalHoursMins(
                                              firstBooking.start_time)},
        .next = DisplayBooking{
            .title = firstTitle,
            .subtitle = timestampToLocalHoursMins(firstBooking.start_time) +
                        std::string(" - ") +
                        timestampToLocalHoursMins(firstBooking.end_time)}};
  }

  // the room is currently occupied
  RoomStatus status = RoomStatus{
      .hasNow = true,
      .now = DisplayBooking{.title = firstTitle, .subtitle = firstSubtitle}};

  bool hasNext = bookings.size() > 1;
  if (hasNext) {
    // the room is currently occupied, and there is a next booking
    Booking secondBooking = bookings[1];
    std::string secondTitle = (secondBooking.notes.length() > 0)
                                  ? secondBooking.notes
                                  : secondBooking.user_name;
    std::string secondSubtitle =
        timestampToLocalHoursMins(secondBooking.start_time) +
        std::string(" - ") + timestampToLocalHoursMins(secondBooking.end_time);
    status.hasNext = true;
    status.next = DisplayBooking{.title = secondBooking.user_name,
                                 .subtitle = secondSubtitle};
  }
  return status;
}

void drawRoomStatus(const RoomStatus& status) {
  int x = 10;
  int cursor_y = 8;
  int line_y = 76;

  display.setFont(ArialMT_Plain_10);
  display.drawString(x, cursor_y, "NOW");
  cursor_y += 10 + 4;

  if (status.hasNow) {
    display.setFont(ArialMT_Plain_24);
    display.drawString(x, cursor_y, status.now.title.c_str());
    cursor_y += 24 + 6;
    display.setFont(ArialMT_Plain_16);
    display.drawString(x, cursor_y, status.now.subtitle.c_str());
  }
  display.drawLine(0, line_y, SCREEN_WIDTH, line_y);
  cursor_y = line_y;
  if (status.hasNext) {
    cursor_y += 6;
    display.setFont(ArialMT_Plain_10);
    display.drawString(x, cursor_y, "NEXT");
    cursor_y += 10 + 2;
    display.setFont(ArialMT_Plain_16);
    display.drawString(x, cursor_y, status.next.title.c_str());
    cursor_y += 16 + 2;
    display.setFont(ArialMT_Plain_10);
    display.drawString(x, cursor_y, status.next.subtitle.c_str());
  }
}

void loop() {
  display.clear();
  display.clear();
  std::vector<Booking> bookings = getBookings(ROOM_ID);

  Serial.println("Bookings:");
  for (const Booking& booking : bookings) {
    printBooking(booking);
  }

  RoomStatus status = getRoomStatus(bookings);
  printRoomStatus(status);

  drawRoomStatus(status);
  display.display();
  delay(10000);
}
