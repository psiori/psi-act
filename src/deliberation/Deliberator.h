//
//  Deliberator.h
//  psi-act
//
//  Sascha Lange, 2020
//

#ifndef __PSI_ACT__DELIBERATOR_H__
#define __PSI_ACT__DELIBERATOR_H__

#include <string>

#include "TimePoint.h"
#include "CycleManager.h"

namespace psi_act {

struct DesireInformation {
  std::string desire;

  TimePoint startedAt;
  TimePoint endedAt;

  unsigned long startedAtCycle;
  unsigned long endedAtCycle;

  DesireInformation(const std::string& desire,
                    const TimePoint& presentTime=CYCLE.startedAt,
                    long presentCycle=CYCLE.counter);
  
  DesireInformation();
};

class Deliberator {
public:
  Deliberator(const std::string& name="AbstractDeliberator");
  virtual ~Deliberator() {};
  
  virtual std::string getPresentDesire() const=0;
  virtual DesireInformation getPresentDesireInformation() const=0;
  virtual DesireInformation getPreviousDesireInformation() const=0;
  
  virtual std::string deliberate(const TimePoint& time)=0;

protected:
  const std::string name;
};

}

#endif /* __PSI_ACT__DELIBERATOR_H__ */

