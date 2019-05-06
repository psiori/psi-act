
#include "Option.h"

psi_act::Option::Option(const std::string& name) : name(name)
{}

bool psi_act::Option::checkCommitmentCondition(const TimePoint&) throw()
{
  return true;
}

bool psi_act::Option::checkInvocationCondition(const TimePoint&) throw()
{
  return true;
}

double psi_act::Option::getSuccessProbability(const TimePoint&) const throw() {
  return 1.;
};
