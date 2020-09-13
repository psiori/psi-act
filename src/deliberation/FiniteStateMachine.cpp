//
//  FiniteStateMachine.cpp
//  psi-act
//
//  Sascha Lange, 2014, 2020
//

#include "FiniteStateMachine.h"

#include <iostream>


// ////////////////////////////////////////////////////////////////////////////
//
//    F I N I T E   S T A T E   M A C H I N E
//
// ////////////////////////////////////////////////////////////////////////////

psi_act::FiniteStateMachine::FiniteStateMachine()
{}


psi_act::FiniteStateMachine::~FiniteStateMachine()
{
  std::cerr << "DEBUG: destructor of state machine called." << std::endl;
  for (auto it : states) {
    delete it.second;
  }
}


psi_act::FiniteStateMachine*
psi_act::FiniteStateMachine::createState(
    const std::string& name,
    std::function<void(void)> onEnter,
    std::function<void(void)> onLeave)
{
  auto it = states.find(name);
  if (it != states.end()) {
    throw std::invalid_argument("state already exists: " + name);
  }
  State* state = states[name] = new State(name, onEnter, onLeave);
  if (!presentState) {
    presentState = state;  // this seems to be the very first state.
  }

  return this;
}


psi_act::FiniteStateMachine*
psi_act::FiniteStateMachine::addTransition(
    const std::string& from,
    const std::string& to,
    std::function<bool(void)> condition)
{
  State* fromState = states[from];
  State* toState = states[to];

  if (!fromState) {
    throw std::invalid_argument("origin state unknown: " + from);
  }
  if (!toState) {
    throw std::invalid_argument("target state unknown: " + to);
  }

  Transition* transition = new Transition(fromState, toState, condition);
  fromState->addTransition(transition);

  return this;
}


psi_act::FiniteStateMachine::State*
psi_act::FiniteStateMachine::execute()
{
  State* targetState = presentState->execute();

  presentState->onLeave();
  presentState = targetState;
  presentState->onEnter();

  return presentState;
}



// ////////////////////////////////////////////////////////////////////////////
//
//    S T A T E
//
// ////////////////////////////////////////////////////////////////////////////


psi_act::FiniteStateMachine::State::State(
    const std::string& name,
    std::function<void(void)> onEnter,
    std::function<void(void)> onLeave)
  : name(name), onEnter(onEnter), onLeave(onLeave)
{}

psi_act::FiniteStateMachine::State::~State()
{
  std::cerr << "DEBUG: destructor of state called: " << name << std::endl;
  for (auto it : transitions) {
    delete it;
  }
}


psi_act::FiniteStateMachine::State*
psi_act::FiniteStateMachine::State::execute()
{
  for (auto transition : transitions) {
    if (transition->condition()) {
      return transition->to;
    }
  }
  std::cerr << "ERROR: no transition condition was met. Stay in present state."
            << std::endl;  // \todo : should we throw an exception?

  return this;
}



// ////////////////////////////////////////////////////////////////////////////
//
//    T R A N S I T I O N
//
// ////////////////////////////////////////////////////////////////////////////

psi_act::FiniteStateMachine::Transition::Transition(
    State* from,
    State* to,
    std::function<bool(void)> condition)
  : from(from), to(to), condition(condition)
{}


psi_act::FiniteStateMachine::Transition::~Transition()
{
  std::cerr << "DEBUG: destructor of unnamed transition called." << std::endl;
}

