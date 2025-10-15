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

  void clear();

  void update();

 private:
  DEPG0290BxS800FxX_BW* display_;

  void VextON();
  void VextOFF();
};

#endif  // DISPLAY_H
