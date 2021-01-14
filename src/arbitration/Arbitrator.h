//
//  Arbitrator.h
//  psi-act
//
//  Sascha Lange, 2003, 2004, 2019
//

/**
* \file Arbitrator.h
* \brief Abstract class to inherit from for the arbitration class in use.
*
* \dir arbitration
* \brief The arbitration is the derivation of the next action to take, taking into account information from the machine under control and goals sent from deliberation.
*/

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
