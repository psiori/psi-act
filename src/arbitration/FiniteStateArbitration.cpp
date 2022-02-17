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

FiniteStateArbitration *FiniteStateArbitration::createState(
    const std::string &name, std::shared_ptr<Option> option,
    std::function<void(void)> onEnter, std::function<void(void)> onLeave) {
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

Command *FiniteStateArbitration::calculateCommand(
    const TimePoint &time) noexcept(false) {
  // Make sure right away that a neutral command is set.
  if (!Command::getNeutralCommand()) {
    throw std::runtime_error("Neutral command for arbitration not set");
  }

  // If no states available do nothing
  if (this->state_machine.getPresentState() == nullptr) {
    return new Command();
  }

  // Check which state is active and get the corresponding option
  std::string cur_state_name = this->currentStateName();
  std::shared_ptr<Option> cur_option =
      this->state_option_map.at(cur_state_name);

  // Execute that option
  return cur_option->calculateCommand(time);
}

void FiniteStateArbitration::cycleStarted(const Cycle &cycle,
                                          const TimePoint &time) noexcept {
  this->state_machine.execute(time);
}

std::string FiniteStateArbitration::currentOptionName() const {
  std::string cur_state_name = this->currentStateName();
  return this->state_option_map.at(cur_state_name)->toString();
}

std::string FiniteStateArbitration::currentStateName() const {
  return this->state_machine.getPresentState()->name;
}

std::string FiniteStateArbitration::listOptions(bool listNested) {
  std::stringstream ss;
  for (std::string &state : this->state_machine.getStates()) {
    ss << this->state_option_map.at(state)->toString() << std::endl;
  }
  return ss.str();
}
