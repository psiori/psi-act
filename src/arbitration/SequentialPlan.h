#ifndef _PSIACT_SEQUENTIAL_PLAN_H_
#define _PSIACT_SEQUENTIAL_PLAN_H_

//
//  SequentialPlan.h
//  psi-act
//
//  Sascha Lange, 2003,2004,2019
//


#include <vector>

#include "psiact/arbitration/Arbitrator.h"

namespace psi_act {
/**
 * @class SequentialPlan
 * @brief Sequential Plan used to define a sequence of Options, each being
 *          executed one after the other.
 */
class SequentialPlan : public Arbitrator {
public:
  SequentialPlan(const std::string& name = "SequentialPlan");

  virtual ~SequentialPlan() noexcept;

  virtual bool
  checkInvocationCondition(const TimePoint& time) noexcept override;
  virtual bool
  checkCommitmentCondition(const TimePoint& time) noexcept override;

  virtual void gainControl(const TimePoint&) noexcept override;
  virtual void loseControl(const TimePoint&) noexcept override;

  virtual void
  cycleStarted(const Cycle& cycle, const TimePoint& time) noexcept override;

  virtual Command* calculateCommand(const TimePoint&) override;

  /**
   * @brief Add an \ref Option with different settings as a stage. The stages
   * will be executed FIFO.
   * @param option            The \ref Option that will be executed.
   * @param skippable         Whether this behavior may be skipped.
   * @param interruptable     Whether this behavior may be interrupted after
   *                          having taken control.
   * @param seekInterruption  Whether this behavior tries to interrupt previous
   *                          behaviors.
   * @param takeOwnership     Make this \ref Arbitrator responsible to free this
   *          \ref Option's memory after usage.
   */
  psi_act::SequentialPlan* appendStage(Option* option,
                                       bool skippable         = false,
                                       bool interruptable     = false,
                                       bool seekInteruption   = false,
                                       bool takeoverOwnership = true) noexcept;

  std::string listOptions(bool listNested = true) noexcept override;

  std::string currentOptionName() const override;

  std::vector<Option*> getOptions() noexcept override {
    std::vector<Option*> opts;
    for (Stage& stage : this->options) {
      opts.emplace_back(stage.option);
    }
    return opts;
  }

private:
  struct Stage {
    Option* option;
    bool skipable;
    bool interruptable;
    bool seekInteruption;
    bool owned;
  };

  std::vector<Stage> options;
  virtual void determineScheduledIntention(const TimePoint& time) noexcept;
  int presentIntention;    ///< index of present stage (or -1) if inactive
  int scheduledIntention;  ///< index of next stage that can be executed (in
                           ///< many cases same as presentIntention)

  bool intentionCompleted;
};

}  // namespace psi_act

#endif
