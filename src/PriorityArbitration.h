#ifndef _PSIACT_PRIORITYARBITRATION_H_
#define _PSIACT_PRIORITYARBITRATION_H_

//
//  PriorityArbitration.h
//  psi-act
//
//  Sascha Lange, 2003,2004,2019
//


#include <string>
#include <vector>

#include "Arbitrator.h"
#include "Command.h"
#include "CycleManager.h"

namespace psi_act {

  class PriorityArbitration : public Arbitrator, CycleNotifiable {
  public:
    PriorityArbitration(const std::string& name="PriorityArbitration");
    virtual ~PriorityArbitration() throw() {};

    virtual Command* calculateCommand(const TimePoint&) throw(PsiActException);

    /** true, if at least one of the available options is invocable */
    virtual bool checkInvocationCondition(const TimePoint& time) throw();
    /** true, if the presently executed option can continue, or at least
      one different option is invocable */
    virtual bool checkCommitmentCondition(const TimePoint& time) throw();

    virtual void cycleStarted(const Cycle& cycle, const TimePoint& executionTime) throw();
    virtual void cycleEnded(const Cycle& cycle, const TimePoint& executionTime) throw();

    virtual void gainControl(const TimePoint&) throw();
    virtual void loseControl(const TimePoint&) throw();

    /** Set and get the neutral command used as a fallback command. */
    virtual void setNeutralCommand(Command* cmd);

    /** Adds the given argument to the end of the list of available options,
      with lowest priority.

     \param interrupatable if true, this option may be interrupte by another
                           option of higher priority.

     \param takeoverOwnership if true, the plan owns the given option and
                              has to take care of freeing its memory
     */
    void addOption (Option* option,
                    bool interruptable=true,
                    bool takeoverOwnership=true) throw ();

  protected:
    virtual void determineUpcomingOption(const TimePoint& time) throw();

    struct OptionWithFlags {
      Option* option;
      bool interrupatable;
      bool owned;
    };
    std::vector<OptionWithFlags> options; ///< in order of decreasing priority

    int upcomingOption; ///<
    int presentOption;
    Command* neutralCommand;
  };


  class AlternativeOptions {
  public:
    // selects and follows the most applicable
  protected:
    std::vector<Option*> options;
  };

}

#endif
