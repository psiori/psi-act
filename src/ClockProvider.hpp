#pragma once

#include <atomic>
#include <csignal>
#include <psiact/TimePoint.h>

namespace psi_act {

/** Supplies matching timestamps and waits for the configured runtime clock. */
class ClockProvider {
public:
  /** Restore the original clock when the active provider is destroyed. */
  virtual ~ClockProvider();

  /** Read the selected clock without changing timestamp types or units. */
  virtual psi_act::TimePoint now() const noexcept = 0;

  /** Wait in this clock's time domain, optionally observing a shutdown flag. */
  virtual void
  sleepFor(psi_act::Duration duration,
           const volatile std::sig_atomic_t* interrupted = nullptr) = 0;

  /** Route psi-act timestamps through this provider. */
  void activate();

private:
  /** Adapt the selected provider to the clock's function-pointer API. */
  static psi_act::TimePoint providerNow() noexcept;

  /** Active provider; its owner must outlive all threads using this clock. */
  static std::atomic<ClockProvider*> active_;
};

}  // namespace psi_act
