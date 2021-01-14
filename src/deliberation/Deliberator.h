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

namespace psi_act
{
  /**
   * DesireInformation contains the desire as string and
   * when to start and stop its emittance.
   */
  struct DesireInformation
  {
    std::string desire;

    TimePoint startedAt;
    TimePoint endedAt;

    unsigned long startedAtCycle;
    unsigned long endedAtCycle;

    DesireInformation(const std::string &desire,
                      const TimePoint &presentTime =
                          CYCLE.executionExpectedAt,
                      long presentCycle = CYCLE.counter);

    DesireInformation();
  };

  /**
  * Deliberator is the base class for any deliberator yet to come. Handles DesireInformation
  * and deliberates the next goal.
  */
  class Deliberator
  {
  public:
    Deliberator(const std::string &name = "AbstractDeliberator");
    virtual ~Deliberator(){};

    virtual std::string getPresentDesire() const = 0;
    virtual DesireInformation getPresentDesireInformation() const = 0;
    virtual DesireInformation getPreviousDesireInformation() const = 0;

    virtual std::string deliberate(const TimePoint &time =
                                       CYCLE.executionExpectedAt) = 0;

    std::string getName() const { return name; }

  protected:
    const std::string name;
  };

} // namespace psi_act

#endif /* __PSI_ACT__DELIBERATOR_H__ */
