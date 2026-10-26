#pragma once

#include "ClockProvider.hpp"

namespace psi_act {

/** Original wall-clock timestamps and sleeps for non-simulation operation. */
class SystemClock : public ClockProvider {
public:
  /** Read wall time directly, without recursing through the active provider. */
  psi_act::TimePoint now() const noexcept override;

  /** Preserve the original wall-time sleep behavior. */
  void
  sleepFor(psi_act::Duration duration,
           const volatile std::sig_atomic_t* interrupted = nullptr) override;
};

}  // namespace psi_act
