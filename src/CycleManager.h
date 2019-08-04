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
    
    CycleManager();
    
    void registerNotifiable(CycleNotifiable* notifyable);
    void unregisterNotifiable(CycleNotifiable* notifyable);
    void unregisterAllNotifables();
    
    void start();
    void advance();
    
    const Cycle& presentCycle();
    
  protected:
    boost::circular_buffer<Cycle> cycles;
    
    std::set<CycleNotifiable*> notifiables;
    
    void endCylce();
    void startCycle();
  };
    
  #define CM (psi_act::CycleManager::getTheCycleManager())

}

#endif /* CycleManager_h */
