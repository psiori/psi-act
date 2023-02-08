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

psi_act::CycleManager::CycleManager(long cycle_time_us,
                                    long execution_delay_us)
  : duration_us(cycle_time_us), delay_us(execution_delay_us), cycles(30) {
}

void psi_act::CycleManager::registerNotifiable(CycleNotifiable* notifiable)
{
  this->notifiables.insert(notifiable);
}

void psi_act::CycleManager::unregisterNotifiable(CycleNotifiable* notifiable)
{
  if (notifiable) {
    this->notifiables.erase(notifiable);
  }
}

void psi_act::CycleManager::unregisterAllNotifables()
{
  this->notifiables.clear();
}

void psi_act::CycleManager::resetWithFrequency(double frequency)
{
  if (frequency <= 0) {
    throw std::invalid_argument("CycleManger::reset called with invalid "
                                "frequency. Must be larger than zero.");
  }
  duration_us = static_cast<std::chrono::microseconds>(
    (long long)(1000/frequency*1000.0));
}


double psi_act::CycleManager::getFrequency() const
{
  return 1000000.0 / duration_us.count();
};



void psi_act::CycleManager::start()
{
  cycles.clear();
  startCycle();
}

void psi_act::CycleManager::endCycle()
{
  if (cycles.size()) {
    Cycle& cycle = cycles.back();
    cycle.endedAt = psi_act::Clock::now();
    cycle.ended = true;
    for (auto it = this->notifiables.begin(); it != this->notifiables.end(); it++) {
      (*it)->cycleEnded(cycle, cycle.executionExpectedAt);
    }
  }
}

void psi_act::CycleManager::startCycle()
{
  Cycle cycle;
  int missed_cycles = 0;

  cycle.startedAt = psi_act::Clock::now();

  if (cycles.size()) {
    cycle.counter = cycles.back().counter + 1;
    cycle.desiredEndAt = cycles.back().desiredEndAt + duration_us;
  }
  else {
    cycle.desiredEndAt = cycle.startedAt + duration_us;
  }

  while (cycle.desiredEndAt <= cycle.startedAt) {
    cycle.desiredEndAt = cycle.desiredEndAt + duration_us;
    ++missed_cycles;
  }

  if (missed_cycles > 0) {
    // TODO(sascha@psiori.com): handle, and output error.
  }

  cycle.executionExpectedAt = cycle.desiredEndAt + delay_us;
  cycles.push_back(cycle);

  for (auto it = this->notifiables.begin(); it != this->notifiables.end(); it++) {
    (*it)->cycleStarted(cycle, cycle.executionExpectedAt);
  }

}

void psi_act::CycleManager::advance()
{
  if (!cycles.size()) {
    this->start();
  }
  else {
    this->endCycle();
    this->startCycle();
  }
}

const psi_act::Cycle& psi_act::CycleManager::currentCycle() {
  if (!cycles.size()) {
    this->start();
  }
  return this->cycles.back();
}

std::chrono::milliseconds psi_act::CycleManager::remainingInCycle() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
    currentCycle().desiredEndAt - psi_act::Clock::now());
}



double psi_act::CycleManager::averageDuration(unsigned long history) const {
  long duration = 0;
  int count     = 0;
  if (history == 0)
  {
      history = cycles.size();
  }
  for (auto it = cycles.begin() + cycles.size() - std::min<long>(history, cycles.size());
       it != cycles.end(); it++) {
    if (it->ended) {
      duration += std::chrono::duration_cast<std::chrono::milliseconds>(
                      it->endedAt - it->startedAt)
                      .count();
      ++count;
    }
  }
  return count > 0 ? duration / (double)count : duration_us.count() / 1000.0;
}

double psi_act::CycleManager::maxDuration(unsigned long history) const {
  long max_duration = 0;
  if (history == 0)
  {
      history = cycles.size();
  }
  for (auto it = cycles.begin() + cycles.size() - std::min<long>(history, cycles.size());
       it != cycles.end(); it++) {
    if (it->ended) {
      max_duration = std::max(max_duration, std::chrono::duration_cast<std::chrono::milliseconds>(
                      it->endedAt - it->startedAt)
                      .count());
    }
  }
  return max_duration / 1000.0;
}
