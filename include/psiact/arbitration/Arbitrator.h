//
//  Arbitrator.h
//  psi-act
//
//  Sascha Lange, 2003, 2004, 2019
//

#ifndef Arbitrator_h
#define Arbitrator_h

#include <string>
#include <vector>

#include "psiact/arbitration/Option.h"

namespace psi_act {

class Arbitrator : public Option {
public:
  Arbitrator(const std::string &name = "Arbitrator");
  ~Arbitrator(){};

  virtual std::string listOptions(bool listNested = true) const noexcept = 0;

  virtual std::vector<Option *> getOptions(void) noexcept = 0;
};

} // namespace psi_act

#endif /* Arbitrator_h */
