#ifndef _PSIACT_SEQUENTIAL_PLAN_H_
#define _PSIACT_SEQUENTIAL_PLAN_H_

//
//  SequentialPlan.h
//  psi-act
//
//  Sascha Lange, 2003,2004,2019
//

/**
 * \file SequentialPlan.h
 * \brief Arbitration of actions by sequence. Each option can be executed one time only.
 */

#include <vector>

#include "Arbitrator.h"

namespace psi_act {
 /**
 * @class SequentialPlan
 * @brief SequentialPlan used to define a sequence of Options, each being
 *          executed one after the other.
 */
class SequentialPlan : public Arbitrator {
public:
  SequentialPlan(const std::string& name = "SequentialPlan");

  virtual ~SequentialPlan() noexcept;

  virtual bool checkInvocationCondition(const TimePoint& time) noexcept;
  virtual bool checkCommitmentCondition(const TimePoint& time) noexcept;

  virtual void gainControl(const TimePoint&) noexcept;
  virtual void loseControl(const TimePoint&) noexcept;

  virtual void cycleStarted(const Cycle& cycle, const TimePoint& time) noexcept;

  virtual Command* calculateCommand(const TimePoint&);

   /**
   * @brief Add an \ref Option with different settings as a stage. The stages
   * will be executed FIFO.
   * @param option            The \ref Option that will be executed.
   * @param skippable         Whether this behavior may be skipped.
   * @param interruptable     Whether this behavior may be interrupted after
   *          having taken control.
   * @param seekInterruption  Whether this behavior tries to interrupt previous
   *          behaviors.
   * @param takeOwnership     Make this \ref Arbitrator responsible to free this
   *          \ref Option's memory after usage.
   */
  void appendStage(Option* option,
                   bool skippable         = false,
                   bool interruptable     = false,
                   bool seekInteruption   = false,
                   bool takeoverOwnership = true) noexcept;

  virtual std::string listOptions(bool listNested = true);
  virtual std::string currentOptionName() const;

private:
  /** Stage inherits from Option to add some additional flags. */
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
