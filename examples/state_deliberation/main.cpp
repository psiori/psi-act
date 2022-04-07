#include <iostream>

#include "deliberation/FiniteStateDeliberation.h"
#include "CycleManager.h"


using namespace psi_act;

int main(int argc, const char* argv[]) {

  FiniteStateDeliberation* deliberation = new FiniteStateDeliberation();

  deliberation->getFiniteStateMachine()

    ->createState("not divisable by 4", []() {
      std::cout << CYCLE.counter << " cannot divided by 4" << std::endl;
    })
    ->createState("divisable by 4", []() {
      std::cout << CYCLE.counter << " can be divided by 4" << std::endl;
    })

    ->addTransition("start", "not divisable by 4")

    ->addTransition("not divisable by 4", "divisable by 4",
                    [](const TimePoint& time) {
      return CYCLE.counter % 4 == 0;
    })
    ->addTransition("not divisable by 4", "exit",
                  [](const TimePoint& time) {
      return CYCLE.counter > 20;
    })
    ->addTransition("not divisable by 4", "not divisable by 4")

    ->addTransition("divisable by 4", "not divisable by 4");

  std::string goal = "";

  while (goal != "exit") {
    // pseudo control loop. Doing one decision / transition per cycle.
    CM.advance();

    std::cerr << "START OF CYCLE " << CYCLE.counter << std::endl;

    goal = deliberation->deliberate();

    std::cerr << "END OF CYCLE " << CYCLE.counter << std::endl;
  }


  delete deliberation;

  return 0;
}
