
#include "SequentialPlan.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

psi_act::SequentialPlan::SequentialPlan(const std::string& name) :
    Arbitrator(name), presentIntention(-1), scheduledIntention(-1) {
}

psi_act::SequentialPlan::~SequentialPlan() noexcept {
  for (std::vector<Stage>::iterator it = options.begin(); it != options.end();
       it++) {
    if (it->owned && it->option) {
      delete it->option;
      it->option = 0;
    }
  }
  options.clear();
}


psi_act::SequentialPlan*
psi_act::SequentialPlan::appendStage(Option* option,
                                     bool skippable,
                                     bool interruptable,
                                     bool seekInterruption,
                                     bool takeoverOwnership) noexcept {
  Stage stage = {
      option, skippable, interruptable, seekInterruption, takeoverOwnership};
  options.push_back(stage);
  return this;
}

void psi_act::SequentialPlan::gainControl(const TimePoint& time) noexcept {
  psi_act::Arbitrator::gainControl(time);
}

void psi_act::SequentialPlan::loseControl(const TimePoint& time) noexcept {
  if (presentIntention >= 0 && presentIntention < options.size()) {
    options[presentIntention].option->loseControl(time);
  }
  presentIntention   = -1;
  scheduledIntention = -1;  // set the "inactive" state
  psi_act::Arbitrator::loseControl(time);
}

void psi_act::SequentialPlan::cycleStarted(const Cycle&,
                                           const TimePoint&) noexcept {
  // reset the scheduled action for this cycle; needs to be reevaluated
  scheduledIntention = -1;
}

bool psi_act::SequentialPlan::checkInvocationCondition(
    const TimePoint& time) noexcept {
  return this->checkCommitmentCondition(time);  // This is correct because
  // of the implementation of check commitment: If there's no present
  // option, it checks for an invocable action. If there's a present
  // option, it means this behavior is already active. In that case,
  // the invocation condition will return the same output as the
  // commitment condition.
}

bool psi_act::SequentialPlan::checkCommitmentCondition(
    const TimePoint& time) noexcept {
  determineScheduledIntention(time);
  return scheduledIntention >= 0 && scheduledIntention < options.size();
}

void psi_act::SequentialPlan::determineScheduledIntention(
    const TimePoint& time) noexcept {
  // Procedure
  //   scheduledIntention is set to the highest stage that can be executed
  //   nextStage is set to the highest invocable stage
  //   continueSearch determines whether the current stage allows interruption,
  //   and search should therefore be continued
  //    --> interruptabl stages are never executed, if a later
  //        stage seeks execution
  //    --> ONLY skippable stages might be left out
  const int stageNum = options.size();
  scheduledIntention =
      stageNum;  // in case no invocable stage is found, set to last Behavior

  bool interruptAllowed = false;
  bool foundInvocable   = false;
  intentionCompleted    = true;
  if (presentIntention >= 0 && presentIntention < stageNum) {
    interruptAllowed = options[presentIntention].interruptable;
    if (options[presentIntention].option->checkCommitmentCondition(time)) {
      foundInvocable =
          true;  // if this stage is commited, no need to search further
      intentionCompleted = false;
      scheduledIntention = presentIntention;
    }
  }

  bool continueSearch = !foundInvocable || interruptAllowed;
  int nextStage       = presentIntention + 1;
  while (continueSearch && nextStage < stageNum) {
    bool mightInterrupt =
        interruptAllowed &&
        options[nextStage]
            .seekInteruption;  // boolean whether the investigated stage might
                               // interrupt the currently running stage
    if ((!foundInvocable || mightInterrupt) &&
        options[nextStage].option->checkInvocationCondition(time)) {
      scheduledIntention =
          nextStage;  // Remember stage, if running stage may be interrupted and
                      // this stage is invocable
      foundInvocable = true;
      if (!mightInterrupt) {
        return;
      }
    }
    continueSearch =
        options[nextStage].skipable;  // continue search, if currently selected
                                      // stage is skipable
    nextStage++;
  }
}

psi_act::Command*
psi_act::SequentialPlan::calculateCommand(const TimePoint& time) {
  // Make sure right away that a neutral command is set.
  if (!Command::getNeutralCommand()) {
    throw std::runtime_error("Neutral command for arbitration not set");
  }

  // If no upcoming option is determined, do so now.
  if (scheduledIntention < 0) {
    determineScheduledIntention(time);
  }

  // Manage present and upcoming Intention lifecycle.
  if (presentIntention != scheduledIntention || intentionCompleted) {
    if (presentIntention >= 0 && presentIntention < options.size()) {
      options[presentIntention].option->loseControl(time);
    }
    if (scheduledIntention >= 0 && scheduledIntention < options.size()) {
      options[scheduledIntention].option->gainControl(time);
    }
  }

  // Make sure arbitrators inheriting from this one adhere to calling
  // gainControl and loseControl, ensuring proper cleanup.
  if (!this->has_control) {
    throw std::runtime_error(this->toString() +
                             std::string("::gainControl was never called."));
  }

  // Store present option although scheduled might be -1
  presentIntention = scheduledIntention;

  if (presentIntention >= 0 && presentIntention < options.size()) {
    scheduledIntention = -1;
    return options[presentIntention].option->calculateCommand(time);
  } else {
    scheduledIntention = -1;
    return Command::getNeutralCommand();
  }
}

std::string psi_act::SequentialPlan::listOptions(
    bool listNested)  // argmunet necessary for required signature, but not used
{
  std::stringstream ss;
  for (unsigned int i = 0; i < options.size(); i++) {
    ss << options[i].option->toString() << std::endl;
  }
  return ss.str();
}

std::string psi_act::SequentialPlan::currentOptionName() const {
  std::string child = "none";
  if (presentIntention >= 0 && presentIntention < options.size()) {
    child = options[presentIntention].option->currentOptionName();
  }
  return toString() + "/" + child;
}
