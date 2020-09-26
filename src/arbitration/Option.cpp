
#include "Option.h"

psi_act::Option::Option(const std::string& name, 
                        bool receiveCycleNotifications)
  : name(name)
{
  if (receiveCycleNotifications) {
    CM.registerNotifiable(this);  // register self to receive notifications
  }
}

psi_act::Option::~Option()
{
  CM.unregisterNotifiable(this);
}



bool psi_act::Option::checkCommitmentCondition(const TimePoint&) throw()
{
  return true;
}

bool psi_act::Option::checkInvocationCondition(const TimePoint&) throw()
{
  return true;
}

double psi_act::Option::getSuccessProbability(const TimePoint&) const throw()
{
  return 1.;
};

void psi_act::Option::cycleStarted(const Cycle& cycle,
                                   const TimePoint& time) throw()
{
}

void psi_act::Option::cycleEnded(const Cycle& cycle,
                                 const TimePoint& time) throw()
{
}
