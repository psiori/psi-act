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

  struct Cycle {
  public:
    long counter;
    bool ended;
    TimePoint startedAt;
    TimePoint desiredEndAt;
    TimePoint executionExpectedAt;
    TimePoint endedAt;
    
    Cycle() : counter(0), ended(false), startedAt(psi_act::Clock::now()) {};
  };

  class CycleNotifiable {
  public:
    virtual void cycleStarted(const Cycle& cycle, const TimePoint& executionTime) throw()=0;
    virtual void cycleEnded(const Cycle& cycle, const TimePoint& executionTime) throw()=0;
    
    virtual ~CycleNotifiable() {};
  };

  class CycleManager {
  public:
    static CycleManager& getTheCycleManager();
    
    CycleManager(long cycle_time_us=33333,
                 long execution_delay_us=50000);
    
    void registerNotifiable(CycleNotifiable* notifyable);
    void unregisterNotifiable(CycleNotifiable* notifyable);
    void unregisterAllNotifables();
    
    void start();
    void advance();
    
    const Cycle& presentCycle();
    double averageDuration() const;
    
    std::chrono::milliseconds remainingInCycle();
    
  protected:
    std::chrono::microseconds duration_us;
    std::chrono::microseconds delay_us;
    
    boost::circular_buffer<Cycle> cycles;
    
    std::set<CycleNotifiable*> notifiables;
    
    void endCycle();
    void startCycle();
  };
    
  #define CM (psi_act::CycleManager::getTheCycleManager())
  #define CYCLE (psi_act::CycleManager::getTheCycleManager().presentCycle())

}

#endif /* CycleManager_h */
