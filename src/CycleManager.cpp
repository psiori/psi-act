//
//  CycleManager.cpp
//  psi-act
//
//  Sascha Lange, 2019
//

#include "CycleManager.h"


psi_act::CycleManager& psi_act::CycleManager::getTheCycleManager() {
  static CycleManager the_cycle_manager;
  return the_cycle_manager;
}

psi_act::CycleManager::CycleManager() : cycles(30) {
}


void psi_act::CycleManager::registerNotifiable(CycleNotifiable* notifiable)
{
  this->notifiables.insert(notifiable);
}

void psi_act::CycleManager::unregisterNotifiable(CycleNotifiable* notifiable)
{
  this->notifiables.erase(notifiable);  
}

void psi_act::CycleManager::unregisterAllNotifables()
{
  this->notifiables.clear();
}

void psi_act::CycleManager::start()
{
  cycles.clear();
  cycles.push_back(Cycle());
}

void psi_act::CycleManager::advance()
{
  if (!cycles.size()) {
    this->start();
  }
  
  Cycle& last_cycle = cycles.back();

  Cycle cycle;
  cycle.counter = last_cycle.counter+1;
  
  cycles.push_back(cycle);  
}

const psi_act::Cycle& psi_act::CycleManager::presentCycle() {
  if (!cycles.size()) {
    this->start();
  }
  return this->cycles.back();
}


