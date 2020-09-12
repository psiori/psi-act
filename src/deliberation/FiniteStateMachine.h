//
//  FiniteStateMachine.h
//  psi-act
//
//  Sascha Lange, 2016, 2020
//

#ifndef __PSI_ACT__FINITE_STATE_MACHINE_H__
#define __PSI_ACT__FINITE_STATE_MACHINE_H__

#include <string>


namespace psi_act {

class FiniteStateMachine {
public:
  class Tranistion;
  class State {
  };
  
  class Transition {
    
  };
  
  FiniteStateMachine();
  virtual ~FiniteStateMachine() {};
  
  FiniteStateMachine* addState(State* state);

  
protected:
};

}

#endif /* __PSI_ACT__FINITE_STATE_MACHINE_H__ */


