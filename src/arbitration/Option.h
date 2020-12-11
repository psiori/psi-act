//
//  Option.h
//  psi-act
//
//  Sascha Lange, 2003,2004,2019
//

#ifndef _PSIACT_OPTION_H_
#define _PSIACT_OPTION_H_

#include <string>
#include <chrono>
#include "Command.h"
#include "CycleManager.h"

namespace psi_act {

  class Option : public CycleNotifiable {
  public:
    Option(const std::string& name="GenericOption",
           bool receiveCycleNotifications=false);

    virtual ~Option();

    virtual bool checkCommitmentCondition(const TimePoint&) noexcept;
    virtual bool checkInvocationCondition(const TimePoint&) noexcept;

    virtual double getSuccessProbability(const TimePoint&) const noexcept;

    virtual Command* calculateCommand(const TimePoint&)=0;
    
    virtual void cycleStarted(const Cycle& cycle, const TimePoint& time) noexcept;
    virtual void cycleEnded(const Cycle& cycle, const TimePoint& time) noexcept;

    virtual void gainControl(const TimePoint&) {}
    virtual void loseControl(const TimePoint&) {}

    virtual std::string toString() const { return name; };
    virtual std::string currentOptionName() const { return toString(); };

  protected:
    std::string name;
  };

}

#endif
