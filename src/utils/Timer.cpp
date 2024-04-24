// ///////////////////////////////////////////////////////////////////////////
//
//   AUTOCRANE
//
//   PSIORI GmbH -- all rights reserved.
//
// ///////////////////////////////////////////////////////////////////////////

/**
 * @file Timer.cpp
 * @authors Alexander Hoereth (alexander@psiori.com)
 * @authors Robin Jehn (robin@psiori.com)
 */

#include "Logging/Timer.hpp"

#include <boost/format.hpp>
#include <chrono>
#include <cmath>
#include <numeric>
#include <queue>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "Logging/Logger.hpp"

namespace psi_act {

TimerStatistics::TimerStatistics(const std::string &name) : name(name) {}

TimerStatistics::~TimerStatistics() { log(); }

void TimerStatistics::newTime(long nanosecs, const std::string &key) {
  if (times.find(key) == times.end()) {
    std::vector<long> cons;
    cons.push_back(nanosecs);
    times[key] = cons;
    keys.push_back(key);
  } else {
    times[key].push_back(nanosecs);
  }
}

void TimerStatistics::log() {
  LOG_TRACE(Logger::POSTPROCESSING, "Information about Timer " << name << ": ");
  // Sort times into heap by how long a function took in total
  [[maybe_unused]] long overall = 0;
  for (const std::string &key : keys) {
    long total =
        std::accumulate(times.at(key).begin(), times.at(key).end(), 0.0);
    overall += total;
    double average = total / times.at(key).size();
    std::string timing_information =
        key + " took " + std::to_string(total) + " nanoseconds in total and " +
        std::to_string(average) + " nanoseconds on average";
    costs.emplace(total, timing_information);
  }

  // Log the statistics
  for (size_t i = 0; i < keys.size(); i++) {
    std::string timing_information = std::get<1>(costs.top());
    costs.pop();
    LOG_TRACE(Logger::POSTPROCESSING, timing_information);
  }

  // As stated in the string the total is not really the total,
  // but I didn't come up with a smarter way yet.
  LOG_TRACE(Logger::POSTPROCESSING,
            "\"" << name << "\" took " << overall
                 << " nanoseconds in total "
                    "(sum of the individual functions, if they call each other "
                    "its higher than the actual total)");
}

void TimerStatistics::reset() {
  keys = std::vector<std::string>();
  times = std::unordered_map<std::string, std::vector<long>>();
}

void TimerStatistics::logAndReset() {
  log();
  reset();
}

/** Single Timer **/
SingleTimer::SingleTimer(const std::string &name)
    : start(std::chrono::high_resolution_clock::now()), name(name) {}

SingleTimer::SingleTimer(const std::string &name, TimerStatistics *timer)
    : start(std::chrono::high_resolution_clock::now()), name(name),
      timer_(timer) {}

SingleTimer::~SingleTimer() {
  stop = std::chrono::high_resolution_clock::now();
  long nanosecs =
      std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start)
          .count();
  if (timer_ == nullptr) {
    LOG_TRACE(Logger::POSTPROCESSING, "Time taken by \"" << name << "\" was "
                                                         << nanosecs
                                                         << " nanoseconds");

  } else {
    timer_->newTime(nanosecs, name);
  }
}

Timer::Timer(double min_alpha) : min_alpha{min_alpha} {}

void Timer::tick() {
  auto now = std::chrono::system_clock::now();
  auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
  this->start_ms = now_ms.time_since_epoch().count();
}

std::string Timer::tock() {
  // Decay alpha from 1 to min_alpha to "calibrate" ems at the start.
  this->alpha = std::max(this->min_alpha, this->alpha * (1 - this->min_alpha));

  auto now = std::chrono::system_clock::now();
  auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
  long stop = now_ms.time_since_epoch().count();
  double elapsed = static_cast<double>(stop - this->start_ms);

  double delta = elapsed - this->ema;
  this->ema = this->ema + this->alpha * delta;
  this->emvar = (1 - this->alpha) * (this->emvar + this->alpha * pow(delta, 2));
  this->emstd = sqrt(this->emvar);
  return this->ToString();
}

std::string Timer::ToString() {
  boost::format fmt("%6.2f +-%.2f");
  return (fmt % this->ema % this->emstd).str();
}

} // namespace psi_act
