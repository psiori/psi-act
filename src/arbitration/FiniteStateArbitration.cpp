#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Command.h"
#include "CycleManager.h"
#include "FiniteStateArbitration.hpp"

using namespace psi_act;

FiniteStateArbitration::FiniteStateArbitration(const std::string &name)
    : Arbitrator(name), state_machine{}, state_option_map{} {}

FiniteStateArbitration::~FiniteStateArbitration() {
  for (std::string &state : this->state_machine.getStates()) {
    delete this->state_option_map[state];
  }
}

FiniteStateArbitration *
FiniteStateArbitration::createState(const std::string &name, Option *option,
                                    std::function<void(void)> onEnter,
                                    std::function<void(void)> onLeave) {

  // Create the state and link the option in the state option map
  this->state_machine.createState(name, onEnter, onLeave);
  this->state_option_map[name] = option;
  return this;
}

FiniteStateArbitration *FiniteStateArbitration::addTransition(
    const std::string &from, const std::string &to,
    const std::string &annotation, const Condition condition) {
  this->state_machine.addTransition(from, to, annotation, condition);
  return this;
}

bool FiniteStateArbitration::checkInvocationCondition(
    const TimePoint &time) noexcept {

  // If no states available
  if (this->state_machine.getStates().size() == 0) {
    return false;
  }

  // If no current option active
  if (this->currentOption() == nullptr) {
    return false;
  }

  return this->currentOption()->checkInvocationCondition(time);
}

bool FiniteStateArbitration::checkCommitmentCondition(
    const TimePoint &time) noexcept {

  // If no states available
  if (this->state_machine.getStates().size() == 0) {
    return false;
  }

  // If no current option active
  if (this->currentOption() == nullptr) {
    return false;
  }

  this->state_machine.execute(time);

  return this->currentOption()->checkCommitmentCondition(time);
}

void FiniteStateArbitration::gainControl(const TimePoint &time) noexcept {}

void FiniteStateArbitration::loseControl(const TimePoint &time) noexcept {
  if (this->state_machine.getStates().size() == 0) {
    return;
  }

  std::string initial_state_name = this->state_machine.getStates()[0];
  this->state_machine.setActiveState(initial_state_name);
}

Command *FiniteStateArbitration::calculateCommand(
    const TimePoint &time) noexcept(false) {

  // Make sure right away that a neutral command is set.
  if (!Command::getNeutralCommand()) {
    throw std::runtime_error("Neutral command for arbitration not set");
  }

  // If no state is active do nothing
  if (this->currentOption() == nullptr) {
    return new Command();
  }

  // Execute the active option
  return this->currentOption()->calculateCommand(time);
}

void FiniteStateArbitration::cycleStarted(const Cycle &cycle,
                                          const TimePoint &time) noexcept {}

Option *FiniteStateArbitration::currentOption() const {
  try {
    return this->state_option_map.at(this->currentStateName());
  } catch (const std::runtime_error &e) {
    return nullptr;
  }
}

std::string FiniteStateArbitration::currentOptionName() const {
  Option *cur_option = this->currentOption();
  return (cur_option == nullptr) ? "" : cur_option->toString();
}

std::string FiniteStateArbitration::currentStateName() const {
  const FiniteStateMachine::State *cur_state =
      this->state_machine.getPresentState();
  if (cur_state == nullptr) {
    throw std::runtime_error("No state available");
  }
  return cur_state->name;
}

std::string FiniteStateArbitration::listOptions(bool listNested) {
  std::stringstream ss;
  for (std::string &state : this->state_machine.getStates()) {
    ss << this->state_option_map.at(state)->toString() << std::endl;
  }
  return ss.str();
}
