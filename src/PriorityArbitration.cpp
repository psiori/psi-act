//
//  PriorityArbitration.cpp
//  psi-act
//

#include "PriorityArbitration.h"

psi_act::PriorityArbitration::PriorityArbitration(const std::string& name)
    : Arbitrator(name), presentOption(-1)
{
}

psi_act::Command* psi_act::PriorityArbitration::calculateCommand(
    const TimePoint& time) throw(PsiActException)
{
  // Make sure right away that a neutral command is set.
  if (!neutralCommand)
  {
    throw PsiActException();  // TODO: Error message.
  }

  // If no upcoming option is determined, do so now.
  if (upcomingOption < 0)
  {
    determineUpcomingOption(time);
  }

  // If no upcoming option is determined, return the neutralCommand.
  if (upcomingOption < 0)
  {
    presentOption = -1;
    return neutralCommand;
  }

  // Manage present and upcoming option lifecycle.
  if (presentOption != upcomingOption)
  {
    options[presentOption].option->loseControl(time);
    options[upcomingOption].option->gainControl(time);
  }

  // Store present option, also it might be -1.
  presentOption = upcomingOption;

  return options[presentOption].option->calculateCommand(time);
}

void psi_act::PriorityArbitration::setNeutralCommand(Command* cmd)
{
  neutralCommand = cmd;
}

void psi_act::PriorityArbitration::addOption(Option* option, bool interruptable,
                                             bool takeoverOwnership) throw()
{
  OptionWithFlags optionWithFlags = {option, interruptable, takeoverOwnership};
  options.push_back(optionWithFlags);
}

void psi_act::PriorityArbitration::cycleStarted(const Cycle& cycle,
                                                const TimePoint& time) throw()
{
}

void psi_act::PriorityArbitration::cycleEnded(const Cycle& cycle,
                                              const TimePoint& time) throw()
{
}

void psi_act::PriorityArbitration::gainControl(const TimePoint& time) throw() {}

void psi_act::PriorityArbitration::loseControl(const TimePoint& time) throw()
{
  options[presentOption].option->loseControl(time);
  presentOption = -1;  // set the "inactive" state
}

bool psi_act::PriorityArbitration::checkInvocationCondition(
    const TimePoint& time) throw()
{
  return this->checkCommitmentCondition(time);  // This is correct because
  // of the implementation of check invocation: If there's no present
  // option, it checks for an invocable action. If there's a present
  // option, it means this behavior is already active. In that case,
  // the invocation condition will return the same output as the
  // commitment condition.
}

bool psi_act::PriorityArbitration::checkCommitmentCondition(
    const TimePoint& time) throw()
{
  determineUpcomingOption(time);
  return upcomingOption >= 0;
}

void psi_act::PriorityArbitration::determineUpcomingOption(
    const TimePoint& time) throw()
{
  // Option for the new cycle has not been determined yeet.
  upcomingOption = -1;

  const bool hasPresentOption = (presentOption >= 0 &&
                                 presentOption <= options.size());


  // If present option is not interruptable and commited to continue, continue.
  if (hasPresentOption && !options[presentOption].interrupatable &&
      options[presentOption].option->checkCommitmentCondition(time))
  {
    // Present option is still committed and is not interruptabel.
    upcomingOption = presentOption;
    return;
  }

  for (unsigned int i = 0; i < options.size(); i++)
  {
    if (i == presentOption &&
        options[presentOption].option->checkCommitmentCondition(time))
    {
      // No higher priority option wants to interrupt present and still
      // committed option.
      upcomingOption = i;
      return;
    }
    if (i != presentOption && options[i].option->checkInvocationCondition(time))
    {
      // A different option wants to be invoked.
      upcomingOption = i;
      return;
    }
  }

  return;
}
