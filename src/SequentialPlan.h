#ifndef _PSIACT_SEQUENTIAL_PLAN_H_
#define _PSIACT_SEQUENTIAL_PLAN_H_

//
//  SequentialPlan.h
//  psi-act
//
//  Sascha Lange, 2003,2004,2019
//


#include <vector>
#include "Arbitrator.h"

namespace psi_act {
  
  /** Sequential plan is used to define a sequence of options, that should
    be executed in sequence, one after the other. */
  class SequentialPlan : public Arbitrator {
  public:
    SequentialPlan(std::string name="SequentialPlan");

    virtual ~SequentialPlan() throw();
    
    virtual bool checkInvocationCondition(const TimePoint& time) throw();
    virtual bool checkCommitmentCondition(const TimePoint& time) throw();
    
    virtual void cycleStarted(const TimePoint&) throw();
    virtual void loseControl(const TimePoint&) throw(PsiActException);

    
    /** append an option to the end of the list of stages. */
    void appendStage (Option* option, 
											bool skippable, 
											bool interruptable, 
											bool takeoverOwnership) throw (std::bad_alloc);
    
  private:
    struct Stage {
      Option* option;
      bool skipable;
      bool interruptable;
      bool owned;
    };
    
    std::vector<Stage> options;
    
    int presentIntention;  ///< index of present stage (or -1) if inactive
    int scheduledIntention;  ///< index of next stage that can be executed

    bool completed;    
  };
  
}

#endif
