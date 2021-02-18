//
//  Option.h
//  psi-act
//
//  Sascha Lange, 2003,2004,2019
//

/**
* \file Option.h
* \brief Comprises of all necessary parts needed to conduct one specific behaviour.
*/

#ifndef _PSIACT_OPTION_H_
#define _PSIACT_OPTION_H_

#include <string>
#include <chrono>
#include "Command.h"
#include "CycleManager.h"

namespace psi_act {
  /**
   * Option contains all information necessary to decide whether a behaviour
   * - the option - can be executed, and what actual command is issued to the
   * controlled machine.
   */
  class Option : public CycleNotifiable
  {
  public:
    Option(const std::string& name="GenericOption",
           bool receiveCycleNotifications=false);

    virtual ~Option();

    /**
     * @brief check whether an option can be continued
     */
    virtual bool checkCommitmentCondition(const TimePoint&) noexcept;
    /**
     * @brief check whether an option can be started
     */
    virtual bool checkInvocationCondition(const TimePoint&) noexcept;

    virtual double getSuccessProbability(const TimePoint&) const noexcept;

    /**
     * @brief computed direct command to the machine like velocities or electrical voltage.
     */
    virtual Command *calculateCommand(const TimePoint&) = 0;

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
// namespace psi_act

#endif
