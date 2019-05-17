//
//  CycleManager.h
//  psi-act
//
//  Sascha Lange, 2019
//

#ifndef CycleManager_h
#define CycleManager_h

#include <set>

#include "psiact_time.h"

namespace psi_act {

  struct Cycle {
  public:
    long counter;
    TimePoint startedAt;
    TimePoint desiredEndAt;
    TimePoint executionExpectedAt;
    TimePoint endedAt;
  };

  class CycleNotifiable {
  public:
    virtual void cycleStarted(const Cycle& cycle, const TimePoint& executionTime) throw()=0;
    virtual void cycleEnded(const Cycle& cycle, const TimePoint& executionTime) throw()=0;
  };

  class CycleManager {
  public:
    static CycleManager& getTheCycleManager();

    void registerNotifiable(CycleNotifiable* notifyable);
    void unregisterNotifiable(CycleNotifiable* notifyable);
    void unregisterAllNotifables();

  protected:
    Cycle* presentCycle;
    Cycle* previousCycle;

    std::set<CycleNotifiable*> notifiables;
  };

  #define CM (CycleManager::getTheCycleManager())

}

#endif /* CycleManager_h */
