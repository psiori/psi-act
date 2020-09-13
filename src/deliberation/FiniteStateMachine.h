//
//  FiniteStateMachine.h
//  psi-act
//
//  Sascha Lange, 2016, 2020
//

#ifndef __PSI_ACT__FINITE_STATE_MACHINE_H__
#define __PSI_ACT__FINITE_STATE_MACHINE_H__

#include <map>
#include <string>
#include <vector>


namespace psi_act {

class FiniteStateMachine {
protected:
  class Transition;
  
public:
  class State {
  public:
    const std::string name;
    
    State(const std::string& name) : name(name) {}
    virtual ~State();
    
    State* execute();
    
    void addTransition(const Transition* transition) {
      transitions.push_back(transition);
    };
      
  protected:
    std::vector<const Transition*> transitions;
  };
  
  FiniteStateMachine();
  virtual ~FiniteStateMachine();

  FiniteStateMachine* createState(const std::string& name);
  FiniteStateMachine* addTransition(
      const std::string& from,
      const std::string& to,
      std::function<bool(void)> condition = []() -> bool { return true; });

  FiniteStateMachine* registerState(State* state);
  
  State* getPresentState() { return presentState; }
  State* execute();

protected:
  class Transition {
  public:
    State* from;
    State* to;
    
    Transition(State* from, State* to, std::function<bool(void)> condition);
    virtual ~Transition();
    
    std::function<bool(void)> condition;
  };

  std::map<std::string, State*> states;
  State* presentState;
};

}

#endif /* __PSI_ACT__FINITE_STATE_MACHINE_H__ */


