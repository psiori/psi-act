//
//  PriorityArbitration.cpp
//  psi-act
//

#include "PriorityArbitration.h"

#include <iostream>
#include <sstream>
#include <stdexcept>


psi_act::PriorityArbitration::PriorityArbitration(const std::string& name)
    : Arbitrator(name), upcomingOption(-1), presentOption(-1)
{}

psi_act::PriorityArbitration::~PriorityArbitration() noexcept
{
  for (std::vector<OptionWithFlags>::iterator it = options.begin();
       it != options.end(); it++)
  {
    if (it->owned && it->option)
    {
      delete it->option;
      it->option = 0;
    }
  }
  options.clear();
}

psi_act::Command* psi_act::PriorityArbitration::calculateCommand(
    const TimePoint& time) noexcept(false)
{
  // Make sure right away that a neutral command is set.
  if (!Command::getNeutralCommand())
  {
    throw std::runtime_error("Neutral command for arbitration not set");
  }

  // If no upcoming option is determined, do so now.
  if (upcomingOption < 0 || upcomingOption >= options.size())
  {
    determineUpcomingOption(time);
  }

  // correct out-of-bounds
  if (upcomingOption >= options.size())
  {
    upcomingOption = -1;
  }

  // Manage present and upcoming option lifecycle.
  if (presentOption != upcomingOption)
  {
    if (presentOption >= 0 && presentOption < options.size())
    {
      options[presentOption].option->loseControl(time);
    }
    if (upcomingOption >= 0 && upcomingOption < options.size())
    {
      options[upcomingOption].option->gainControl(time);
    }
  }

  // Store present option although upcomingOption might be -1
  presentOption = upcomingOption;

  if (presentOption >= 0)
  {
    return options[presentOption].option->calculateCommand(time);
  }
  else
  {
    return Command::getNeutralCommand();
  }
}

psi_act::PriorityArbitration*
psi_act::PriorityArbitration::addOption(Option* option,
                                        bool interruptable,
                                        bool takeoverOwnership) noexcept {
  OptionWithFlags optionWithFlags = {option, interruptable, takeoverOwnership};
  options.push_back(optionWithFlags);
  return this;
}

std::string psi_act::PriorityArbitration::listOptions(bool listNested)
{
  std::stringstream ss;
  for (unsigned int i = 0; i < options.size(); i++)
  {
    ss << options[i].option->toString() << std::endl;
  }
  return ss.str();
}

void psi_act::PriorityArbitration::gainControl(const TimePoint& time) noexcept
{}

void psi_act::PriorityArbitration::loseControl(const TimePoint& time) noexcept
{
  if (presentOption >= 0 && presentOption < options.size()) {
    options[presentOption].option->loseControl(time);
  }
  presentOption = -1;  // set the "inactive" state
}

bool psi_act::PriorityArbitration::checkInvocationCondition(
    const TimePoint& time) noexcept
{
  return this->checkCommitmentCondition(time);  // This is correct because
  // of the implementation of check commitment: If there's no present
  // option, it checks for an invocable action. If there's a present
  // option, it means this behavior is already active. In that case,
  // the invocation condition will return the same output as the
  // commitment condition.
}

bool psi_act::PriorityArbitration::checkCommitmentCondition(
    const TimePoint& time) noexcept
{
  determineUpcomingOption(time);
  return upcomingOption >= 0 && upcomingOption < options.size();
}

void psi_act::PriorityArbitration::determineUpcomingOption(
    const TimePoint& time) noexcept
{
  // Option for the new cycle has not been determined yet.
  upcomingOption = -1;

  const bool hasPresentOption = (presentOption >= 0 &&
                                 presentOption < options.size());


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
    // by (present) intention, the logic implemented here will
    // not continue to execute a presentOption if it's
    // committmentCondition is false but it's invocation condition
    // is true
    if (i != presentOption && options[i].option->checkInvocationCondition(time))
    {
      // A different option wants to be invoked.
      upcomingOption = i;
      return;
    }
  }

  return;
}

std::string psi_act::PriorityArbitration::currentOptionName() const {
  std::string child = "none";
  if (presentOption >= 0 && presentOption < options.size()) {
    child = options[presentOption].option->currentOptionName();
  }
  return toString() + "/" + child;
}
