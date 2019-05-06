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

namespace psi_act {

  class Option {
  public:
    Option(const std::string& name="GenericOption");
    ~Option() throw () {};

    virtual bool checkCommitmentCondition(const TimePoint&) throw();
    virtual bool checkInvocationCondition(const TimePoint&) throw();

    virtual double getSuccessProbability(const TimePoint&) const throw();

    virtual Command calcuateCommand(const TimePoint&) throw(PsiActException)=0;

    virtual void gainControl(const TimePoint&) throw(PsiActException) {}
    virtual void loseControl(const TimePoint&) throw(PsiActException) {}

    virtual std::string toString() const { return name; };

  protected:
    std::string name;
  };

}

#endif
