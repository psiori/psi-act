#include "Option.h"

psi_act::Option::Option(const std::string& name,
                        bool receiveCycleNotifications) :
    name(name) {
  if (receiveCycleNotifications) {
    CM.registerNotifiable(this);  // register self to receive notifications
  }
}

psi_act::Option::~Option() {
  CM.unregisterNotifiable(this);
}

void psi_act::Option::gainControl(const TimePoint& time) {
  if (this->has_control) {
    throw std::runtime_error(this->toString() +
                             std::string("::loseControl was never called."));
  }
  this->has_control = true;
}

void psi_act::Option::loseControl(const TimePoint& time) {
  if (!this->has_control) {
    throw std::runtime_error(this->toString() +
                             std::string("::gainControl was never called."));
  }
  this->has_control = false;
}

bool psi_act::Option::checkCommitmentCondition(const TimePoint&) noexcept {
  return true;
}

bool psi_act::Option::checkInvocationCondition(const TimePoint&) noexcept {
  return true;
}

double psi_act::Option::getSuccessProbability(const TimePoint&) const noexcept {
  return 1.;
};

void psi_act::Option::cycleStarted(const Cycle& cycle,
                                   const TimePoint& time) noexcept {
}

void psi_act::Option::cycleEnded(const Cycle& cycle,
                                 const TimePoint& time) noexcept {
}
