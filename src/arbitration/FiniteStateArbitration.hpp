// ///////////////////////////////////////////////////////////////////////////
//
//   PSI_ACT
//
//   PSIORI GmbH -- all rights reserved.
//
// ///////////////////////////////////////////////////////////////////////////

/**
 * @file FiniteStateArbitration.hpp
 * @brief Arbitrator that executes chooses between different Options using a
 * Finite State Machine
 * @author Giulio Neusch-Frediani <giulio@psiori.com>
 */
#pragma once

#include "Arbitrator.h"
#include "Command.h"
#include "FiniteStateMachine.h"
#include <string>

namespace psi_act {

using Condition = std::function<bool(const TimePoint &)>;

/**
 * @brief Arbitrator using a finite state machine to choose between different
 * Options.
 */
class FiniteStateArbitration : public Arbitrator {

public:
  /**
   * @brief Create a State corresponding to a given Option
   *
   * @param name name of the state (must be unique)
   * @param option Option to choose when state is active
   * @param onEnter Callback when entering the state
   * @param onLeave Callback when leaving the state
   */
  FiniteStateArbitration *createState(
      const std::string &name, std::shared_ptr<Option> option,
      std::function<void(void)> onEnter = []() {},
      std::function<void(void)> onLeave = []() {});

  /**
   * @brief Add a transition between the Option's states
   *
   * @param from name of the state to transition from
   * @param to name of the state to transition to
   * @param annotation (optional) description of the transition
   * @param condition (optional) condition for the transition to happen
   */
  FiniteStateArbitration *addTransition(
      const std::string &from, const std::string &to,
      const std::string &annotation = "",
      const Condition condition = [](const TimePoint &) -> bool {
        return true;
      });

  FiniteStateArbitration(const std::string &name = "FiniteStateArbitration");
  virtual ~FiniteStateArbitration() noexcept;
  virtual Command *calculateCommand(const TimePoint &) noexcept(false);
  virtual bool checkInvocationCondition(const TimePoint &time) noexcept;
  virtual bool checkCommitmentCondition(const TimePoint &time) noexcept;
  virtual void gainControl(const TimePoint &time) noexcept;
  virtual void loseControl(const TimePoint &time) noexcept;
  virtual void cycleStarted(const Cycle &cycle, const TimePoint &time) noexcept;
  virtual std::string currentOptionName() const;
  virtual std::string listOptions(bool listNested = true);

protected:
  /** State machine handling the substates */
  FiniteStateMachine state_machine;

  /** Mapping from state names to Options */
  std::map<std::string, std::shared_ptr<Option>> state_option_map;

  /**
   * @brief Returns the current option
   * @return shared pointer to the current option or nullptr
   */
  std::shared_ptr<Option> currentOption() const;

  /**
   * @brief Returns the current state name
   * @return shared pointer to the current option or empty string
   */
  std::string currentStateName() const;
};

} // namespace psi_act
