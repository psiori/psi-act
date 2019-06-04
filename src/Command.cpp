//
//  Command.cpp
//  psi-act
//
//  Sascha Lange 2019.
//

#include "Command.h"

psi_act::Command* psi_act::Command::the_neutral_command = 0;
bool psi_act::Command::neutral_command_owned = false;

void psi_act::Command::setNeutralCommand(psi_act::Command* command,
                                                bool own)
{
  if (Command::the_neutral_command && Command::neutral_command_owned) {
    delete Command::the_neutral_command;
  }
  Command::the_neutral_command = command;
  Command::neutral_command_owned = own;
}

psi_act::Command* psi_act::Command::getNeutralCommand()
{
  if (!Command::the_neutral_command) {
    // TODO(sascha@psiori.com): throw exception ?
  }
  return Command::the_neutral_command;
}
