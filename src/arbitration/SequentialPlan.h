#ifndef _PSIACT_SEQUENTIAL_PLAN_H_
#define _PSIACT_SEQUENTIAL_PLAN_H_

//
//  SequentialPlan.h
//  psi-act
//
//  Sascha Lange, 2003,2004,2019
//


#include <vector>

#include "Arbitrator.h"

namespace psi_act {
/**
 * @class SequentialPlan
 * @brief Sequential plan is used to define a sequence of options, that should
  be executed in sequence, one after the other. */
class SequentialPlan : public Arbitrator {
public:
  SequentialPlan(const std::string& name = "SequentialPlan");

  virtual ~SequentialPlan() throw();

  virtual bool checkInvocationCondition(const TimePoint& time) throw();
  virtual bool checkCommitmentCondition(const TimePoint& time) throw();

  virtual void gainControl(const TimePoint&) throw();
  virtual void loseControl(const TimePoint&) throw();


  virtual Command* calculateCommand(const TimePoint&) throw();


  /**
   * @brief Add an behavior with different options as a stage. The stages will
   * be executed FIFO
   * @param option The Behavior that will be executed
   * @param skippable whether this behavior may be skipped
   * @param interruptable whether this behavior may be interrupted after
   * starting
   * @param seekInterruption  whether this behavior tries to interrupt previous
   * behaviors
   * @param takeOwnership make this Planner responsible to free the memory after
   * usage */
  void appendStage(Option* option,
                   bool skippable,
                   bool interruptable,
                   bool seekInteruption,
                   bool takeoverOwnership) throw();


  virtual std::string listOptions(bool listNested = true);


private:
  struct Stage {
    Option* option;
    bool skipable;
    bool interruptable;
    bool seekInteruption;
    bool owned;
  };

  std::vector<Stage> options;
  virtual void determineScheduledIntention(const TimePoint& time) throw();
  int presentIntention;    ///< index of present stage (or -1) if inactive
  int scheduledIntention;  ///< index of next stage that can be executed (in
                           ///< many cases same as presentIntention)

  bool intentionCompleted;
};

}  // namespace psi_act

#endif
