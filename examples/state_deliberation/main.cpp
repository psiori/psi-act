#include <iostream>

#include "deliberation/FiniteStateDeliberation.h"
#include "CycleManager.h"


using namespace psi_act;

int main(int argc, const char* argv[]) {

  FiniteStateDeliberation* deliberation = new FiniteStateDeliberation();

  /*
  automaton
    ->createState("not divisable by 4")
    ->createState("divisable by 4", []() {
      std::cout << "(Whoop, whoop! I've just entered the divisable state :) ";
    });

  automaton
    ->addTransition("not divisable by 4", "divisable by 4",
                    []() -> bool { return CYCLE.counter % 4 == 0; })
    ->addTransition("not divisable by 4", "not divisable by 4")
    ->addTransition("divisable by 4", "not divisable by 4");

  do {  // pseudo control loop. Doing one decision / transition per cycle.
    CM.advance();
    std::cerr << "START OF CYCLE " << CYCLE.counter << std::endl;
    std::cout
      << CYCLE.counter
      << " is "
      << automaton->execute()->name
      << std::endl;
    std::cerr << "END OF CYCLE " << CYCLE.counter << std::endl;
  } while (CYCLE.counter < 10);

  */

  delete deliberation;

  return 0;
}
