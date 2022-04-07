//
//  FiniteStateDeliberator.cpp
//  psi-act
//
//  Sascha Lange, 2020
//

#include "FiniteStateDeliberation.h"

#include <iostream>

psi_act::FiniteStateDeliberation::FiniteStateDeliberation(
    const std::string& name) : Deliberator(name)
{
  fsm = new FiniteStateMachine();

  fsm->createState("start")
     ->createState("exit");

  fsm->addTransition("exit", "exit");  // make exit absorbing

  presentDesire = previousDesire = DesireInformation("start");
}


psi_act::FiniteStateDeliberation::~FiniteStateDeliberation()
{
  delete fsm;
};


std::string psi_act::FiniteStateDeliberation::getPresentDesire() const
{
  return presentDesire.desire;
}


psi_act::DesireInformation
psi_act::FiniteStateDeliberation::getPresentDesireInformation() const
{
  return presentDesire;
}


psi_act::DesireInformation
psi_act::FiniteStateDeliberation::getPreviousDesireInformation() const
{
  return previousDesire;
}


std::string
psi_act::FiniteStateDeliberation::deliberate(const TimePoint& time)
{
  FiniteStateMachine::State* state = fsm->execute(time);

  if (state->name != presentDesire.desire) {
    presentDesire.endedAt = time;
    presentDesire.endedAtCycle = CYCLE.counter;
    previousDesire = presentDesire;

    presentDesire = DesireInformation(state->name, time, CYCLE.counter);
  }

  return state->name;
}
