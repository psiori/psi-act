//
//  FiniteStateMachine.h
//  psi-act
//
//  Sascha Lange, 2014, 2020
//

#ifndef __PSI_ACT__FINITE_STATE_MACHINE_H__
#define __PSI_ACT__FINITE_STATE_MACHINE_H__

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "CycleManager.h"
#include "TimePoint.h"


namespace psi_act {

class FiniteStateMachine {
protected:
  class Transition;
  
public:
  class State {
  public:
    const std::string name;
    
    State(const std::string& name,
          std::function<void(void)> onEnter,
          std::function<void(void)> onLeave);
    virtual ~State();
    
    State* execute(const TimePoint& time);
    
    void addTransition(const Transition* transition) {
      transitions.push_back(transition);
    };

    std::function<void(void)> onEnter;
    std::function<void(void)> onLeave;

    std::vector<const Transition*> transitions;
  };
  
  FiniteStateMachine();
  virtual ~FiniteStateMachine();

  FiniteStateMachine* createState(const std::string& name,
                                  std::function<void(void)> onEnter = []() {},
                                  std::function<void(void)> onLeave = []() {});

  FiniteStateMachine* addTransition(
      const std::string& from,
      const std::string& to,
      const std::string& annotation,
      std::function<bool(const TimePoint&)> condition =
          [](const TimePoint&) -> bool { return true; });

  FiniteStateMachine* addTransition(
      const std::string& from,
      const std::string& to,
      std::function<bool(const TimePoint&)> condition =
          [](const TimePoint&) -> bool { return true; });

  FiniteStateMachine* registerState(State* state);
  
  State* getPresentState() { return presentState; }
  State* execute(const TimePoint& time = CYCLE.startedAt);

  /**
   * @brief Produce a graphviz dot format graph of the FSM.
   * Can be rendered using https://dreampuf.github.io/GraphvizOnline
   */
  std::string toGraphviz(void) const;

protected:
  class Transition {
  public:
    State* from;
    State* to;
    std::string annotation;

    Transition(State* from,
               State* to,
               std::function<bool(const TimePoint& time)> condition,
               const std::string& annotation = "");
    virtual ~Transition();

    std::function<bool(const TimePoint& time)> condition;
  };

  std::map<std::string, State*> states;
  State* presentState;
};

}  // namespace psi_act

#endif /* __PSI_ACT__FINITE_STATE_MACHINE_H__ */


