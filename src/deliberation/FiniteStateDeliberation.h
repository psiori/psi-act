//
//  FiniteStateDeliberator.h
//  psi-act
//
//  Sascha Lange, 2020
//

/**
 * \file FiniteStateDeliberation.h
 * \brief Deliberator following the rules of a finite state machine.
 */

#ifndef __PSI_ACT__FINITESTATEDELIBERATION_H__
#define __PSI_ACT__FINITESTATEDELIBERATION_H__

#include <string>

#include "Deliberator.h"
#include "FiniteStateMachine.h"
#include "TimePoint.h"

namespace psi_act {
/**
 * FiniteStateDeliberation inherits from Deliberator and FiniteStateMachine.
 * It acts as a deliberator with a finite set of goals, of which only one can
 * be emitted at a time.
 */
class FiniteStateDeliberation : public Deliberator {
public:
  FiniteStateDeliberation(const std::string& name = "FiniteStateDeliberation");
  virtual ~FiniteStateDeliberation();

  virtual std::string getPresentDesire() const;
  virtual DesireInformation getPresentDesireInformation() const;
  virtual DesireInformation getPreviousDesireInformation() const;

  virtual std::string
  deliberate(const TimePoint& time = CYCLE.executionExpectedAt);

  virtual FiniteStateMachine* getFiniteStateMachine() {
    return fsm;
  };
  virtual const FiniteStateMachine* getFiniteStateMachine() const {
    return fsm;
  };

protected:
  FiniteStateMachine* fsm;
  DesireInformation presentDesire;
  DesireInformation previousDesire;
};

}  // namespace psi_act

#endif /* __PSI_ACT__FINITESTATEDELIBERATION_H__ */
