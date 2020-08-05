//
//  CycleManager.h
//  psi-act
//
//  Sascha Lange, 2019
//

#ifndef CycleManager_h
#define CycleManager_h

#include <set>
#include <boost/circular_buffer.hpp>

#include "Time.h"

namespace psi_act {

  /**
   * Cycle represents a cycle of the control loop and holds information about
   * the timing of its real start, desired end, real end, and expected
   * execution time of the command that is computed and emitted during this
   * cycle. Expected execution time is the desired end time of the cycle
   * (as the command is emitted near the cycle end) plus the average delay
   * that the system needs to really execute this command physically. e.g.
   * by changing the voltage of the motors and thus changing the accelaration.
   * All filters and behaviors should always do all their calculations and
   * decisions for the expected execution time of the command. The cycle
   * also features a counter for the number of cycles that have been executed
   * so far.
   *
   * @attention: Counter may overflow, if the system is run for months without
   * restart.
   */
  struct Cycle {
  public:
    long counter;                   ///< counter, incremented with every cylce
    bool ended;                     ///< true, if the cycle has been ended
    TimePoint startedAt;            ///< real start time of the cycle
    TimePoint desiredEndAt;         ///< desired end time of the cycle
    TimePoint executionExpectedAt;  ///< time the command will take effect
    TimePoint endedAt;              ///< time, the cycle has really ben ended

    Cycle() : counter(0), ended(false), startedAt(psi_act::Clock::now()) {};
  };


  /**
   * CycleNotifiable is an abstract interface that all classes need to
   * implement that want to observe the start and / or the end of cycles.
   * These classes also need to register with the cycle manager to receive
   * notifications.
   */
  class CycleNotifiable {
  public:
    virtual void cycleStarted(const Cycle& cycle, const TimePoint& executionTime) throw()=0;
    virtual void cycleEnded(const Cycle& cycle, const TimePoint& executionTime) throw()=0;

    virtual ~CycleNotifiable() {};
  };

  /**
   * CycleManager is a singleton that manages the cycles of the central.
   * control loop. It keeps log of the current and a few past cycles, does
   * the bookkeeping of cycle start and end times, setting up an instance
   * of Cycle appropriately at the start of each cycle and measuring its
   * end time. The cycle manager also notifies all registered observers
   * at the start of a new cycle and does the same for observers of the
   * cycle end. The Cycle Manager needs to be incorporated in the central
   * control loop of the system, calling start() before the very first
   * cycle and advance() at the very end of each cycle before the start
   * of the next cycle.
   */
  class CycleManager {
  public:
    static CycleManager& getTheCycleManager();

    void registerNotifiable(CycleNotifiable* notifiable);
    void unregisterNotifiable(CycleNotifiable* notifiable);
    void unregisterAllNotifables();

    void start();
    void advance();

    const Cycle& currentCycle();


    /**
     * @param   history The number of recent cycles to consider. Pass 0 for global
     * average
     */
    double averageDuration(unsigned long history = 100) const;

    std::chrono::milliseconds remainingInCycle();

  protected:
    CycleManager(long cycle_time_us=33333,
                 long execution_delay_us=50000);
    CycleManager(const CycleManager&) {
      // probably should even throw exception
    }

    std::chrono::microseconds duration_us;
    std::chrono::microseconds delay_us;

    boost::circular_buffer<Cycle> cycles;

    std::set<CycleNotifiable*> notifiables;

    void endCycle();
    void startCycle();
  };

  /// TODO(sascha@psiori.com): we should not have these macros here, but in an appropriate place in the autocrane-core. We should not spam the global scope with non-namespaced symbols nor names
  #define CM (psi_act::CycleManager::getTheCycleManager())
  #define CYCLE (psi_act::CycleManager::getTheCycleManager().currentCycle())

}

#endif /* CycleManager_h */
