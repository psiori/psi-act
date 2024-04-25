
#include "psiact/arbitration/Option.h"

psi_act::Option::Option(const std::string& name,
                        bool receiveCycleNotifications) :
    name(name), receiveCycleNotifications(receiveCycleNotifications) {
  if (receiveCycleNotifications) {
    CM.registerNotifiable(this);  // register self to receive notifications
  }
}

psi_act::Option::~Option() {
  if (receiveCycleNotifications) {
    CM.unregisterNotifiable(this);
  }
}



bool psi_act::Option::checkCommitmentCondition(const TimePoint&) noexcept
{
  return true;
}

bool psi_act::Option::checkInvocationCondition(const TimePoint&) noexcept
{
  return true;
}

double psi_act::Option::getSuccessProbability(const TimePoint&) const noexcept
{
  return 1.;
};

void psi_act::Option::cycleStarted(const Cycle& cycle,
                                   const TimePoint& time) noexcept
{
}

void psi_act::Option::cycleEnded(const Cycle& cycle,
                                 const TimePoint& time) noexcept
{
}
