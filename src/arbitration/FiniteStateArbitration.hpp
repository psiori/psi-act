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

#include <string>

#include "Arbitrator.h"
#include "psiact/Command.h"
#include "psiact/deliberation/FiniteStateMachine.h"

namespace psi_act {

using Condition =
    std::function<bool(const TimePoint&, const bool old_state_commitable)>;

/**
 * @brief Arbitrator using a finite state machine to choose between different
 *        Options.
 *
 * Options can be added to the finite state machine using
 * `createState`. Transitions between the options can be added using
 * `addTransition`. The state machine will initially enter the state created
 * first and use its `calculateCommand` method to act. By calling
 * `checkCommitmentCondition` the state machine will check the conditions
 * provided in the lambda of every transition of the current state and perform
 * the transition whenever this result is `True` as well as the commitment
 * condition of the active state's option being false and the invocation
 * condition of the destination state being true. The transition checks are done
 * in the same order they where added to the state machine. The arbitrator
 * can only invoke/commit if at least one option is provided through the
 * `createState` interface. Otherwise it uses the options commitment condition
 * to determine if to keep commiting.
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
   * @param is_terminal_state Whether the state is terminal and should decommit
   * the arbitrator after being done.
   */
  FiniteStateArbitration* createState(
      const std::string& name,
      Option* option,
      std::function<void(void)> onEnter = []() {},
      std::function<void(void)> onLeave = []() {},
      const bool is_terminal_state      = false);

  /**
   * @brief Add a transition between the Option's states
   *
   * @param from name of the state to transition from
   * @param to name of the state to transition to
   * @param annotation (optional) description of the transition
   * @param condition (optional) condition for the transition to happen. Default
   * commits if the active behavior does not want to commit anymore. Also, the
   * new behaviour always needs to be able to invoke!
   */
  FiniteStateArbitration* addTransition(
      const std::string& from,
      const std::string& to,
      const std::string& annotation = "",
      const Condition condition     = [](const psi_act::TimePoint& t,
                                     const bool old_commitable) -> bool {
        return !old_commitable;
      });

  FiniteStateArbitration(const std::string& name = "FiniteStateArbitration");
  virtual ~FiniteStateArbitration() override;
  virtual Command* calculateCommand(const TimePoint&) override;
  virtual bool
  checkInvocationCondition(const TimePoint& time) noexcept override;
  virtual bool
  checkCommitmentCondition(const TimePoint& time) noexcept override;
  virtual void gainControl(const TimePoint& time) noexcept override;
  virtual void loseControl(const TimePoint& time) noexcept override;
  virtual void
  cycleStarted(const Cycle& cycle, const TimePoint& time) noexcept override;
  std::string currentOptionName() const override;
  std::string listOptions(bool listNested = true) const noexcept override;

  std::vector<Option*> getOptions() noexcept override;

protected:
  /** State machine handling the substates */
  FiniteStateMachine state_machine;

  /** Mapping from state names to Options */
  std::map<std::string, Option*> state_option_map;

  /**
   * @brief Returns the current option
   * @return pointer to the current option or nullptr
   */
  Option* currentOption() const;

  /**
   * @brief Returns the current state name
   * @return shared pointer to the current option or empty string
   */
  std::string currentStateName() const;
};

}  // namespace psi_act
