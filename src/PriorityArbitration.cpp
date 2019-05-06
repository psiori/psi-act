
#include "PriorityArbitration.h"

psi_act::PriorityArbitration::PriorityArbitration(const std::string& name)
  : Arbitrator(name), presentOption(-1)
{}

void psi_act::PriorityArbitration::cycleStarted(const Cycle& cycle, const TimePoint& executionTime) throw()
{}

void psi_act::PriorityArbitration::cycleEnded(const Cycle& cycle, const TimePoint& executionTime) throw()
{
  this->presentOption = -1;  // set the "inactive" state
}

bool psi_act::PriorityArbitration::checkInvocationCondition(const TimePoint& time) throw()
{
  return this->checkCommitmentCondition(time); // this is correct because
  // of the implementation of check invocation: if there's no present
  // option, it checks for an invocable action. if there's a present
  // option, it means this behavior is already active. In that case,
  // the invocation condition will return the same output as the
  // commitment condition.
}

bool psi_act::PriorityArbitration::checkCommitmentCondition(const TimePoint& time) throw()
{
  if (presentOption >= 0 && presentOption <= this->options.size() &&
      this->options[presentOption].option->checkCommitmentCondition(time)) {
    return true;  // present action wants to continue
  }
  
  for (unsigned int i=0; i < this->options.size(); i++) {
    if (i != this->presentOption &&
        this->options[i].option->checkInvocationCondition(time)) {
      return true;  // a different(!) action wants to be invocated
    }
  }
  
  return false;
}
