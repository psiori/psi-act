//
//  FiniteStateMachine.h
//  psi-act
//
//  Sascha Lange, 2014, 2020
//

/**
 * \file FiniteStateMachine.h
 * \brief Logical construct, that contains a set number of states, of which only one can be active at a time.
 */

#ifndef __PSI_ACT__FINITE_STATE_MACHINE_H__
#define __PSI_ACT__FINITE_STATE_MACHINE_H__

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "psiact/CycleManager.h"
#include "psiact/TimePoint.h"


namespace psi_act {
  /**
 * FiniteStateMachine handles states and their transitions.
 */
class FiniteStateMachine {
protected:
  class Transition;

public:
    /**
     * State contains information about its name, functions,
     * when to execute and also about possible transitions
    */
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

  FiniteStateMachine* setTransitionFunction(
      std::function<void(const State& previousState, const State& nextState)>
          onTransition);

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

  /** Get names of all registered states. */
  std::vector<std::string> getStates() const;

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
  std::function<void(const State& previousState, const State& nextState)>
      onTransition = [](const State& previousState, const State& nextState) {
      };
};

} // namespace psi_act

#endif  /* __PSI_ACT__FINITE_STATE_MACHINE_H__ */

