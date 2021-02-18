//
//  Command.h
//  psi-act
//
//  Sascha Lange 2019.
//

/**
 * \file Command.h
 * \brief Abstract class to send a command to the machine under control.
 */

#ifndef _PSIACT_COMMAND_H_
#define _PSIACT_COMMAND_H_

#include <stdexcept>

#include "TimePoint.h"

namespace psi_act
{
/** PsiActException can be thrown when errors specific to psi_act occur. */
class PsiActException : std::runtime_error
{
public:
  PsiActException(const std::string& msg) : std::runtime_error(msg) {}
};

/**
 * Command is the base class for all Commands created by the Decision Making.
 * Users need to subclass command and to implement an application specific
 * Command class which the application's options fill with appropriate
 * values for the actions to take.
 */
class Command
{
public:
  virtual ~Command() {};

  /** sets the neutral command used as a fallback command. */
  static void setNeutralCommand(Command* cmd, bool own=true);
  /** gets the neutral command used as a fallback command. */
  static Command* getNeutralCommand();

protected:
  static Command* the_neutral_command;
  static bool neutral_command_owned;
};

} // namespace psi_act


#endif /* _PSIACT_COMMAND_H_ */
