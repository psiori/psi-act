#include "FiniteStateArbitration.hpp"

#include <functional>
#include <iostream>
#include <psiact/CycleManager.h>
#include <psiact/deliberation/FiniteStateMachine.h>
#include <sstream>
#include <string>
#include <vector>

#include "psiact/Command.h"

using namespace psi_act;

FiniteStateArbitration::FiniteStateArbitration(const std::string& name) :
    Arbitrator(name), state_machine{}, state_option_map{} {
}

FiniteStateArbitration::~FiniteStateArbitration() {
  for (std::string& state : this->state_machine.getStates()) {
    delete this->state_option_map[state];
  }
}

FiniteStateArbitration*
FiniteStateArbitration::createState(const std::string& name,
                                    Option* option,
                                    std::function<void(void)> onEnter,
                                    std::function<void(void)> onLeave) {
  // Create the state and link the option in the state option map
  this->state_machine.createState(name, onEnter, onLeave);
  this->state_option_map[name] = option;
  return this;
}

FiniteStateArbitration*
FiniteStateArbitration::addTransition(const std::string& from,
                                      const std::string& to,
                                      const std::string& annotation,
                                      const Condition condition) {
  const auto combined_condition =
      [*this, to, from, condition](const TimePoint& time) -> bool {
    // Enforce that the new "to" state is invokable, but pass the commitment
    // condition of "from" to the condition so that the caller can decide if to
    // respect it or not.
    const bool to_invokable =
        this->state_option_map.at(to)->checkInvocationCondition(time);
    const bool from_commitable =
        this->state_option_map.at(from)->checkCommitmentCondition(time);
    return to_invokable && condition(time, from_commitable);
  };

  this->state_machine.addTransition(from, to, annotation, combined_condition);
  return this;
}

bool FiniteStateArbitration::checkInvocationCondition(
    const TimePoint& time) noexcept {
  // If no current option active
  if (this->currentOption() == nullptr) {
    return false;
  }

  return this->currentOption()->checkInvocationCondition(time);
}

bool FiniteStateArbitration::checkCommitmentCondition(
    const TimePoint& time) noexcept {
  // If no current option active
  if (this->currentOption() == nullptr) {
    return false;
  }

  this->state_machine.execute(time);
  return this->currentOption()->checkCommitmentCondition(time);
}

void FiniteStateArbitration::gainControl(const TimePoint& time) noexcept {
}

void FiniteStateArbitration::loseControl(const TimePoint& time) noexcept {
  if (this->state_machine.getStates().size() == 0) {
    return;
  }

  std::string initial_state_name = this->state_machine.getStates()[0];
  this->state_machine.setActiveState(initial_state_name);
}

Command* FiniteStateArbitration::calculateCommand(
    const TimePoint& time) noexcept(false) {
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

void FiniteStateArbitration::cycleStarted(const Cycle& cycle,
                                          const TimePoint& time) noexcept {
}

Option* FiniteStateArbitration::currentOption() const {
  try {
    return this->state_option_map.at(this->currentStateName());
  } catch (const std::runtime_error& e) {
    return nullptr;
  }
}

std::string FiniteStateArbitration::currentOptionName() const {
  Option* cur_option = this->currentOption();
  return (cur_option == nullptr) ? "NONE" : cur_option->toString();
}

std::string FiniteStateArbitration::currentStateName() const {
  const FiniteStateMachine::State* cur_state =
      this->state_machine.getPresentState();
  return (cur_state == nullptr) ? "NONE" : cur_state->name;
}

std::string
FiniteStateArbitration::listOptions(bool listNested) const noexcept {
  std::stringstream ss;
  for (std::string& state : this->state_machine.getStates()) {
    ss << this->state_option_map.at(state)->toString() << std::endl;
  }
  return ss.str();
}

std::vector<Option*> FiniteStateArbitration::getOptions() noexcept {
  std::vector<Option*> opts;
  for (const std::string& name : this->state_machine.getStates()) {
    opts.emplace_back(state_option_map[name]);
  }
  return opts;
}
