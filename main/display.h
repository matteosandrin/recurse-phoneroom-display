#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>

#include "models.h"

// Forward declaration to avoid including the problematic library header
class DEPG0290BxS800FxX_BW;

class Display {
 public:
  Display();
  ~Display();

  void init();

  void drawWifiDebug();

  void drawRoomStatus(const RoomStatus& status);

  void drawSadMac(std::string message);

  void drawHappyMac();

  void clear();

  void update();

 private:
  DEPG0290BxS800FxX_BW* display_;

  void drawRoomIcon(int room_id);
  void drawStringLimit(int x, int y, std::string str, int width);
  void drawIconWithMessage(std::string message, int width, int height,
                           const uint8_t* bits);
  void VextON();
  void VextOFF();
};

#endif  // DISPLAY_H
