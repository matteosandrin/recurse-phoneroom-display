#ifndef CONFIG_H
#define CONFIG_H

#define DIRECTION ANGLE_0_DEGREE
#define SCREEN_WIDTH 296
#define SCREEN_HEIGHT 128

#define NPT_SERVER "time.google.com"
#define API_URL "https://phoneroom.recurse.com/api/bookings"

#define ROOM_ID_GREEN 1
#define ROOM_ID_LOVELACE 2

#define ROOM_ID ROOM_ID_LOVELACE
#define ROOM_NAME "Lovelace"

// TODO(msandrin) handle daylight savings in timezone
#define TZ_OFFSET -14400  // -04:00 (Eastern Daylight Time)

#define PIN_RST 5
#define PIN_DC 4
#define PIN_CS 3
#define PIN_BUSY 6
#define PIN_SCK 2
#define PIN_MOSI 1
#define PIN_MISO -1
#define PIN_FREQUENCY 6000000
#define PIN_VEXT 18
#define REFRESH_INTERVAL 10000  // 10 seconds

#endif  // CONFIG_H
