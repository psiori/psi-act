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

