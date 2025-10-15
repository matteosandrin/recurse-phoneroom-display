#include "display.h"

#include <Arduino.h>

#include "HT_DEPG0290BxS800FxX_BW.h"
#include "config.h"
#include "icons/icons.h"
#include "secrets.h"

Display::Display() : display_(nullptr) {}

Display::~Display() {
  // Destructor - clean up display object
  if (display_ != nullptr) {
    delete display_;
    display_ = nullptr;
  }
  VextOFF();
}

void Display::init() {
  VextON();
  delay(100);

  display_ =
      new DEPG0290BxS800FxX_BW(PIN_RST, PIN_DC, PIN_CS, PIN_BUSY, PIN_SCK,
                               PIN_MOSI, PIN_MISO, PIN_FREQUENCY);

  display_->init();
  display_->screenRotate(DIRECTION);
  this->clear();
  delay(1000);
}

void Display::drawWifiDebug() {
  display_->drawString(0, 0, "init >>> ");
  Serial.print("Connecting to ");
  display_->drawString(0, 20, "Connecting to ... ");
  Serial.println(WIFI_SSID);
  display_->drawString(100, 20, WIFI_SSID);
  this->update();
}

void Display::drawRoomStatus(const RoomStatus& status) {
  int x = 10;
  int cursor_y = 8;
  int line_y = 76;

  display_->setFont(ArialMT_Plain_10);
  display_->drawString(x, cursor_y, "NOW");
  cursor_y += 10 + 4;

  if (status.hasNow) {
    display_->setFont(ArialMT_Plain_24);
    display_->drawString(x, cursor_y, status.now.title.c_str());
    cursor_y += 24 + 6;
    display_->setFont(ArialMT_Plain_16);
    display_->drawString(x, cursor_y, status.now.subtitle.c_str());
  }

  cursor_y = line_y;
  display_->drawLine(0, cursor_y, SCREEN_WIDTH, line_y);
  cursor_y += 6;

  display_->setFont(ArialMT_Plain_10);
  display_->drawString(x, cursor_y, "NEXT");
  cursor_y += 10 + 2;

  if (status.hasNext) {
    display_->setFont(ArialMT_Plain_16);
    display_->drawString(x, cursor_y, status.next.title.c_str());
    cursor_y += 16 + 2;
    display_->setFont(ArialMT_Plain_10);
    display_->drawString(x, cursor_y, status.next.subtitle.c_str());
  }
  this->drawRoomIcon(ROOM_ID);
  this->update();
}

void Display::clear() { display_->clear(); }

void Display::update() { display_->display(); }

void Display::drawRoomIcon(int room_id) {
  if (room_id == ROOM_ID_LOVELACE) {
    display_->drawXbm(SCREEN_WIDTH - lovelace_logo_width, 0,
                      lovelace_logo_width, lovelace_logo_height,
                      lovelace_logo_bits);
  }
}

void Display::VextON() {
  pinMode(PIN_VEXT, OUTPUT);
  digitalWrite(PIN_VEXT, HIGH);
}

void Display::VextOFF() {
  pinMode(PIN_VEXT, OUTPUT);
  digitalWrite(PIN_VEXT, LOW);
}
