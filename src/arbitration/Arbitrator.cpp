//
//  Arbitrator.cpp
//  psi-act
//
//  Sascha Lange, 2003, 2004, 2019.
//

#include "Arbitrator.h"

#include "psiact/ConfigNode.hpp"


namespace psi_act {

Arbitrator::Arbitrator(const std::string& name) : Option(name) {
}


void Arbitrator::configure(const ConfigNode& config_node) noexcept {
  for (Option* option : this->getOptions()) {
    option->configure(config_node);
  }
}

}  // namespace psi_act
