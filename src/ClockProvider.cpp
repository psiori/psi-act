#include "ClockProvider.hpp"

namespace psi_act {

std::atomic<ClockProvider*> ClockProvider::active_{nullptr};

ClockProvider::~ClockProvider() {
  // Keep subsequent library calls on the original clock after shutdown.
  if (active_.load() == this) {
    psi_act::Clock::setProvider(nullptr);
    active_.store(nullptr);
  }
}

void ClockProvider::activate() {
  // Install the adapter before constructing timestamped agents and timers.
  active_.store(this);
  psi_act::Clock::setProvider(&providerNow);
}

psi_act::TimePoint ClockProvider::providerNow() noexcept {
  const auto* provider = active_.load();
  return provider ? provider->now() : std::chrono::system_clock::now();
}

}  // namespace psi_act
