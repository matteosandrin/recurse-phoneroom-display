#ifndef MODELS_H
#define MODELS_H

#include <string>

#include "time.h"

struct Booking {
  std::string id;
  std::string user_id;
  std::string user_name;
  std::string room_id;
  std::string room_name;
  time_t start_time;
  time_t end_time;
  std::string notes;
};

struct DisplayBooking {
  std::string title;
  std::string subtitle;
};

struct RoomStatus {
  DisplayBooking now;
  DisplayBooking next;
};

#endif  // MODELS_H
