#include <iostream>

#include "deliberation/FiniteStateMachine.h"
#include "CycleManager.h"

/** This is a demo of the FiniteStateMachine that demonstrates construction
 * of the transition graph and executing it, switching between states once per
 * cycle.
 *
 * It is then used to output whether or not the present cycle counter can be
 * divided by 4 or not.
 */


using namespace psi_act;

int main(int argc, const char* argv[]) {

  FiniteStateMachine* automaton = new FiniteStateMachine();
  // \note: the code below is an automaton, as it has a side effect at each
  //        transition (onEnter). Most effects are void, but when entering
  //        the divisable step, it outputs some text.


  // Create two states, one for counter values that can be divided by 4
  // without rest, and one for counter values that cannot be divided by 4.

  automaton
    ->createState("not divisable by 4")
    ->createState("divisable by 4", []() {
      std::cout << "(Whoop, whoop! I've just entered the divisable state :) ";
    });

  // Now, we want to connect these states in such a way that the automaton
  // is always in the state that matches the present cycle's counter value.

  // This is done by adding 3 transitions:
  // 1) will switch from not divisable to divisable on a condition passed as
  //    lambda. You could also pass a function object.
  // 2) switches from not divisable back to itself, not divisable, otherwise,
  //    relying on the default condition, which is always true. This
  //    works, because transitions' conditions are checked according to
  //    priority, which is defined by sequence of adding.
  // 3) will switch from divisable to not divisable, always, again, by relying
  //    on the default condition.

  automaton
    ->addTransition("not divisable by 4", "divisable by 4",
                    [](const TimePoint& time)
                        -> bool { return CYCLE.counter % 4 == 0; })
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

  delete automaton;
  return 0;
}
