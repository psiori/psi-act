//  FiniteStateMachine.cpp
//  psi-act
//
//  Sascha Lange, 2014, 2020
//

#include "FiniteStateMachine.h"

#include <functional>
#include <iostream>


// ////////////////////////////////////////////////////////////////////////////
//
//    F I N I T E   S T A T E   M A C H I N E
//
// ////////////////////////////////////////////////////////////////////////////

psi_act::FiniteStateMachine::FiniteStateMachine()
  : states(), presentState(nullptr)
{}


psi_act::FiniteStateMachine::~FiniteStateMachine()
{
#ifdef PSIACT_DEBUG
  std::cerr << "DEBUG: destructor of state machine called." << std::endl;
#endif
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


psi_act::FiniteStateMachine* psi_act::FiniteStateMachine::addTransition(
    const std::string& from,
    const std::string& to,
    std::function<bool(const TimePoint& time)> condition,
    const std::string& annotation) {
  State* fromState = states[from];
  State* toState   = states[to];
  if (!fromState) {
    throw std::invalid_argument("origin state unknown: " + from);
  }
  if (!toState) {
    throw std::invalid_argument("target state unknown: " + to);
  }
  fromState->addTransition(
      new Transition(fromState, toState, condition, annotation));
  return this;
}


psi_act::FiniteStateMachine::State*
psi_act::FiniteStateMachine::execute(const TimePoint& time)
{
  State* targetState = presentState->execute(time);

  presentState->onLeave();
  presentState = targetState;
  presentState->onEnter();

  return presentState;
}


std::string psi_act::FiniteStateMachine::toGraphviz(void) const {
  // Open digraph.
  std::string text = std::string("digraph FSM {\n  rankdir=LR;\n");
  // Consuming states.
  text += "  node [shape = doublecircle];\n ";
  for (const auto& state : this->states) {
    if (state.second->transitions.size() == 0) {
      text += " \"" + state.second->name + "\"";
    } else if (state.second->transitions.size() == 1) {
      const auto* transition = state.second->transitions[0];
      if (transition->from == transition->to) {
        text += " " + state.second->name;
      }
    }
  }
  text += ";\n";
  // Non-consuming states.
  text += "  node [shape = circle];\n";
  // Transitions.
  for (const auto& state : this->states) {
    for (const auto& transition : state.second->transitions) {
      text += "  \"" + transition->from->name + "\" -> \"" +
              transition->to->name + "\" [ label = \"" +
              transition->annotation + "\" ];\n";
    }
  }
  // Close digraph.
  text += "}\n";
  return text;
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
#ifdef PSIACT_DEBUG
  std::cerr << "DEBUG: destructor of state called: " << name << std::endl;
#endif
  for (auto it : transitions) {
    delete it;
  }
}


psi_act::FiniteStateMachine::State*
psi_act::FiniteStateMachine::State::execute(const TimePoint& time)
{
  for (auto transition : transitions) {
    if (transition->condition(time)) {
      return transition->to;
    }
  }
#ifdef PSIACT_DEBUG
  std::cerr << "ERROR: no transition condition was met. Stay in present state."
            << std::endl;  // \todo : should we throw an exception?
#endif

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
    std::function<bool(const TimePoint& time)> condition,
    const std::string& annotation) :
    from(from), to(to), condition(condition), annotation(annotation) {
}


psi_act::FiniteStateMachine::Transition::~Transition()
{
#ifdef PSIACT_DEBUG
  std::cerr << "DEBUG: destructor of unnamed transition called." << std::endl;
#endif
}
