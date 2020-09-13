//
//  FiniteStateDeliberator.h
//  psi-act
//
//  Sascha Lange, 2020
//

#ifndef __PSI_ACT__FINITESTATEDELIBERATION_H__
#define __PSI_ACT__FINITESTATEDELIBERATION_H__

#include <string>

#include "TimePoint.h"
#include "Deliberator.h"
#include "FiniteStateMachine.h"

namespace psi_act {

class FiniteStateDeliberation : public Deliberator {
public:
  FiniteStateDeliberation(const std::string& name="FiniteStateDeliberation");
  virtual ~FiniteStateDeliberation();
  
  virtual std::string getPresentDesire() const;
  virtual DesireInformation getPresentDesireInformation() const;
  virtual DesireInformation getPreviousDesireInformation() const;
  
  virtual std::string deliberate(const TimePoint& time);

  virtual FiniteStateMachine* getFiniteStateMachine() { return fsm; };
  virtual const FiniteStateMachine* getFiniteStateMachine() const {
    return fsm;
  };

protected:
  FiniteStateMachine* fsm;
  DesireInformation presentDesire;
  DesireInformation previousDesire;
};

}

#endif /* __PSI_ACT__FINITESTATEDELIBERATION_H__ */
