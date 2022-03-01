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

class ConfigNode;

class Arbitrator : public Option {
public:
  Arbitrator(const std::string& name = "Arbitrator");
  ~Arbitrator(){};

  /**
   * Configure all options managed by this arbitrator.
   */
  virtual void configure(const ConfigNode&) noexcept override;

  virtual std::string listOptions(bool listNested = true) noexcept = 0;

  virtual std::vector<Option*> getOptions(void) noexcept = 0;
};

}  // namespace psi_act

#endif /* Arbitrator_h */
