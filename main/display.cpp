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
  int logo_width = 28;
  int width_limit = SCREEN_WIDTH - (x * 2 + logo_width);

  display_->setFont(ArialMT_Plain_10);
  this->drawStringLimit(x, cursor_y, std::string("NOW"), width_limit);
  cursor_y += 10 + 4;

  display_->setFont(ArialMT_Plain_24);
  this->drawStringLimit(x, cursor_y, status.now.title, width_limit);
  cursor_y += 24 + 6;
  display_->setFont(ArialMT_Plain_16);
  this->drawStringLimit(x, cursor_y, status.now.subtitle, width_limit);

  cursor_y = line_y;
  display_->drawLine(0, cursor_y, SCREEN_WIDTH, line_y);
  cursor_y += 6;

  display_->setFont(ArialMT_Plain_10);
  this->drawStringLimit(x, cursor_y, std::string("NEXT"), width_limit);
  cursor_y += 10 + 2;

  display_->setFont(ArialMT_Plain_16);
  this->drawStringLimit(x, cursor_y, status.next.title, width_limit);
  cursor_y += 16 + 2;
  display_->setFont(ArialMT_Plain_10);
  this->drawStringLimit(x, cursor_y, status.next.subtitle, width_limit);

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

void Display::drawError() {
  int x = (SCREEN_WIDTH - error_icon_width - lovelace_logo_width) / 2;
  int y = (SCREEN_HEIGHT - error_icon_height) / 2;
  display_->drawXbm(x, y, error_icon_width, error_icon_height, error_icon_bits);
  this->drawRoomIcon(ROOM_ID);
  this->update();
}

void Display::drawStringLimit(int x, int y, std::string str, int width) {
  // If the string fits within the width limit, draw it as-is
  if (display_->getStringWidth(str.c_str()) <= width) {
    display_->drawString(x, y, str.c_str());
    return;
  }

  // String is too wide, shorten it with ellipsis
  std::string ellipsis = "...";
  int ellipsis_width = display_->getStringWidth(ellipsis.c_str());

  // Start with the full string and remove characters one at a time
  for (int len = str.length() - 1; len > 0; len--) {
    std::string truncated = str.substr(0, len) + ellipsis;
    if (display_->getStringWidth(truncated.c_str()) <= width) {
      display_->drawString(x, y, truncated.c_str());
      return;
    }
  }

  // If even a single character + ellipsis doesn't fit, just draw the ellipsis
  display_->drawString(x, y, ellipsis.c_str());
}

void Display::VextON() {
  pinMode(PIN_VEXT, OUTPUT);
  digitalWrite(PIN_VEXT, HIGH);
}

void Display::VextOFF() {
  pinMode(PIN_VEXT, OUTPUT);
  digitalWrite(PIN_VEXT, LOW);
}
