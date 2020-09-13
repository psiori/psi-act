//
//  FiniteStateMachine.cpp
//  psi-act
//
//  Sascha Lange, 2016, 2020.
//

#include "FiniteStateMachine.h"

#include <iostream>

psi_act::FiniteStateMachine::FiniteStateMachine()
{}


psi_act::FiniteStateMachine*
psi_act::FiniteStateMachine::createState(const std::string& name)
{
  std::map<std::string, State*>::iterator it = states.find(name);
  if (it != states.end()) {
    std::cerr << "WARNING: already existing state is overriden." << std::endl;
  }
  State* state = states[name] = new State(name);
  if (!presentState) {
    presentState = state;  // this seems to be the very first state.
  }
  
  return this;
}

psi_act::FiniteStateMachine*
psi_act::FiniteStateMachine::addTransition(
    const std::string& from,
    const std::string& to,
    std::function<bool(void)> condition) {
  State* fromState = states[from];
  State* toState = states[to];

  new Transition(fromState, toState, condition);  // registers iteself with fromState
  return this;
}

psi_act::FiniteStateMachine::State*
psi_act::FiniteStateMachine::execute() {
  return presentState = presentState->execute();
}

psi_act::FiniteStateMachine::Transition::Transition(
    State* from,
    State* to,
    std::function<bool(void)> condition)
  : from(from), to(to), condition(condition)
{
  from->addTransition(this);
}

psi_act::FiniteStateMachine::State*
psi_act::FiniteStateMachine::State::execute() {
  for (auto transition : transitions) {
    if (transition->condition()) {
      return transition->to;
    }
  }
  std::cerr << "ERROR: no transition condition was met. Stay in present state."
            << std::endl;
  
  return this;
}



