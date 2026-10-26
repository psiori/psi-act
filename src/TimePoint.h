//
//  Command.h
//  psi-act
//
//  Sascha Lange 2019.
//

#ifndef _PSIACT_TIME_H_
#define _PSIACT_TIME_H_

#include <atomic>
#include <chrono>

namespace psi_act {

using SystemTimePoint = std::chrono::system_clock::time_point;

using Duration = std::chrono::system_clock::duration;
// Select the source of now(); inheritance preserves system_clock's types,
// duration units and Unix epoch, so existing timestamp APIs stay compatible.
//
// Normal operation:
//   Clock::now();  // No provider installed: returns system_clock::now().
//
// Accelerated simulation (install before constructing agents or timers):
//   Clock::setProvider(&simulationNow);
//   Clock::now();  // Calls simulationNow(), returning the latest simulator
//   time.
// Here simulationNow is a time_point (*)() function fed by simulator updates;
// this class does not advance time or multiply elapsed wall time itself.
// At 2x RTF, 50 ms of simulator time elapses in about 25 ms of wall time.
// Both sensor timestamps and simulationNow() must use the same Unix anchor.
//
// Clock::setProvider(nullptr) restores wall time. The provider is shared by
// all threads using this Clock, but does not change system_clock::now() or
// std::this_thread::sleep_for(); simulation-aware waits are handled separately.
class Clock : public std::chrono::system_clock {
public:
  using Provider = time_point (*)();

  static time_point now() noexcept {
    const auto provider = provider_.load(std::memory_order_acquire);
    return provider ? provider() : std::chrono::system_clock::now();
  }

  static void setProvider(Provider provider) noexcept {
    provider_.store(provider, std::memory_order_release);
  }

private:
  inline static std::atomic<Provider> provider_{nullptr};
};
using TimePoint = std::chrono::system_clock::time_point;

}  // namespace psi_act

#endif /* _PSIACT_TIME_H_ */
