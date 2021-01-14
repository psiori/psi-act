//
//  Arbitrator.h
//  psi-act
//
//  Sascha Lange, 2003, 2004, 2019
//

#ifndef Arbitrator_h
#define Arbitrator_h

#include <string>
#include "Option.h"

namespace psi_act
{

  /**
   * Arbitrator is the intermediate class between Option and
   * either SequentialPlan or PriorityArbitration. Does not add
   * any functionality so far.
   */
  class Arbitrator : public Option
  {
  public:
    Arbitrator(const std::string &name = "Arbitrator");
    ~Arbitrator(){};

    virtual std::string listOptions(bool listNested = true) = 0;
  };

} // namespace psi_act

#endif /* Arbitrator_h */
