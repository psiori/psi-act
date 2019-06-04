#include "Time.h"

namespace psi_act {
 TimePoint TimePoint::now() {
  return TimePoint(std::chrono::system_clock::now().time_since_epoch());
 }
}
