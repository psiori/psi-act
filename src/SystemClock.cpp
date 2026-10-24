#include "SystemClock.hpp"

#include <thread>

namespace psi_act {

psi_act::TimePoint SystemClock::now() const noexcept {
  return std::chrono::system_clock::now();
}

void SystemClock::sleepFor(psi_act::Duration duration,
                           const volatile std::sig_atomic_t*) {
  std::this_thread::sleep_for(duration);
}

}  // namespace psi_act
