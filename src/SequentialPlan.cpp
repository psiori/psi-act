
#include "SequentialPlan.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

psi_act::SequentialPlan::SequentialPlan(const std::string& name) :
    Arbitrator(name), presentIntention(-1), scheduledIntention(-1) {
}

psi_act::SequentialPlan::~SequentialPlan() throw() {
  for (std::vector<Stage>::iterator it = options.begin(); it != options.end();
       it++) {
    if (it->owned && it->option) {
      delete it->option;
      it->option = 0;
    }
  }
  options.clear();
}


void psi_act::SequentialPlan::appendStage(Option* option,
                                          bool skippable,
                                          bool interruptable,
                                          bool seekInterruption,
                                          bool takeoverOwnership) throw() {
  Stage stage = {
      option, skippable, interruptable, seekInterruption, takeoverOwnership};
  options.push_back(stage);
}

void psi_act::SequentialPlan::gainControl(const TimePoint& time) throw() {
}

void psi_act::SequentialPlan::loseControl(const TimePoint& time) throw() {
  if (presentIntention >= 0 && presentIntention < options.size()) {
    options[presentIntention].option->loseControl(time);
  }
  presentIntention   = -1;
  scheduledIntention = -1;  // set the "inactive" state
}

bool psi_act::SequentialPlan::checkInvocationCondition(
    const TimePoint& time) throw() {
  return this->checkCommitmentCondition(time);  // This is correct because
  // of the implementation of check commitment: If there's no present
  // option, it checks for an invocable action. If there's a present
  // option, it means this behavior is already active. In that case,
  // the invocation condition will return the same output as the
  // commitment condition.
}

bool psi_act::SequentialPlan::checkCommitmentCondition(
    const TimePoint& time) throw() {
  determineScheduledIntention(time);
  return scheduledIntention >= 0 && scheduledIntention < options.size();
}

void psi_act::SequentialPlan::determineScheduledIntention(
    const TimePoint& time) throw() {
  // Procedure
  //   scheduledIntention is set to the highest stage available
  //   nextStage is set to the highest invocable stage
  //   continueSearch determines wether the current stage allows interruption,
  //   and search should therefore be continued
  //    --> interruptable stages are never invoked, if a later stage is
  //    invocable
  const int stageNum = options.size();
  scheduledIntention = stageNum;  // in case no invocable stage is found

  bool interruptAllowed  = false;
  bool continuationFound = false;
  intentionCompleted     = true;
  if (presentIntention >= 0 && presentIntention < stageNum) {
    interruptAllowed = options[presentIntention].interruptable;
    if (options[presentIntention].option->checkCommitmentCondition(time)) {
      continuationFound =
          true;  // if this stage is commited, no need to search further
      intentionCompleted = false;
      scheduledIntention = presentIntention;
    }
  }

  bool continueSearch = !continuationFound || interruptAllowed;
  int nextStage       = presentIntention + 1;
  while (continueSearch && nextStage < stageNum) {
    bool mightInterrupt =
        interruptAllowed &&
        options[nextStage]
            .seekInteruption;  // boolean wether the investigated stage might
                               // interrupt the currently running stage
    if ((!continuationFound || mightInterrupt) &&
        options[nextStage].option->checkInvocationCondition(time)) {
      scheduledIntention =
          nextStage;  // Remember stage, if running stage may be interrupted and
                      // this stage is invocable
      continuationFound = true;
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
psi_act::SequentialPlan::calculateCommand(const TimePoint& time) throw() {
  // Make sure right away that a neutral command is set.
  if (!Command::getNeutralCommand()) {
    throw std::runtime_error("Neutral command for arbitration not set");
  }

  // If no upcoming option is determined, do so now.
  if (scheduledIntention < 0 || scheduledIntention >= options.size()) {
    determineScheduledIntention(time);
  }

  // correct out-of-bounds
  if (scheduledIntention >= options.size()) {
    scheduledIntention = -1;
  }

  // Manage present and upcoming Intention lifecycle.
  if (presentIntention != scheduledIntention) {
    if (presentIntention >= 0 && presentIntention < options.size()) {
      options[presentIntention].option->loseControl(time);
    }
    if (scheduledIntention >= 0 && scheduledIntention < options.size()) {
      options[scheduledIntention].option->gainControl(time);
    }
  }

  // Store present option although scheduled might be -1
  presentIntention = scheduledIntention;

  if (presentIntention >= 0) {
    return options[presentIntention].option->calculateCommand(time);
  } else {
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
