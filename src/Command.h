//
//  Command.h
//  psi-act
//
//  Sascha Lange 2019.
//

#ifndef _PSIACT_COMMAND_H_
#define _PSIACT_COMMAND_H_

#include <stdexcept>

#include "Time.h"

namespace psi_act
{

class PsiActException : std::runtime_error
{
 public:
  PsiActException(const std::string& msg) : std::runtime_error(msg) {}
};
class Command
{
 public:
  virtual ~Command(){};
};

}  // namespace psi_act

#endif /* _PSIACT_COMMAND_H_ */
