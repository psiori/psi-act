//
//  CycleManager.h
//  psi-act
//
//  Sascha Lange, 2019
//

#ifndef CycleManager_h
#define CycleManager_h

#include "Time.h"

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

  class CylceManager {
  public:
  protected:
    Cycle* presentCycle;
    Cycle* previousCycle;
  };

}

#endif /* CycleManager_h */
