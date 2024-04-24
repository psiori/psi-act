// ///////////////////////////////////////////////////////////////////////////
//
//   AUTOCRANE
//
//   PSIORI GmbH -- all rights reserved.
//
// ///////////////////////////////////////////////////////////////////////////

/**
 *  @file Timer.hpp
 *  @authors Alexander Hoereth (alexander@psiori.com)
 *  @authors Robin Jehn (robin@psiori.com)
 */

#pragma once

#include <chrono>
#include <queue>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace psi_act {

/**
 * @brief The TimerStatistics class measures multiple runs of the same function.
 * If you pass it to a SingleTimer at construction, the TimerStatistics will be
 * notified of how long that single execution took and store that information.
 * By calling the log function it will log statistics about the running time
 * of the individual functions sorted by how long they each took in total. Be
 * aware that we use the string used to construct the SingleTimer in order to
 * store the information, so you should not use the same string multiple times
 * or it will break the functionality.
 *
 */
class TimerStatistics {
private:
  std::string name;
  std::unordered_map<std::string, std::vector<long>> times;
  std::vector<std::string> keys;
  std::priority_queue<std::tuple<long, std::string>> costs;

public:
  explicit TimerStatistics(const std::string &name);
  ~TimerStatistics();

  /**
   * @brief We get the information from the SingleTimer
   * that function with name key has been executed and
   * took nanosecs
   *
   * @param nanosecs
   * @param key
   */
  void newTime(long nanosecs, const std::string &key);

  /**
   * @brief Log timing statistics
   *
   */
  void log();

  /**
   * @brief Delete measured times
   *
   */
  void reset();

  /**
   * @brief Log statistics before resetting
   *
   */
  void logAndReset();
};

/**
 * @brief Instead of having individual TimerStatistics objects,
 * we could make it a static member of the class that would
 * only be destructed after the program has finished
 * executing and then logging timing information.
 *
 */
class SingleTimer {
private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start;
  std::chrono::time_point<std::chrono::high_resolution_clock> stop;
  std::string name;
  TimerStatistics *timer_ = nullptr;

public:
  /**
   * @brief Timing how long a function takes
   * add "SingleTimer _(__func__);" at the top of the function
   *
   * @param name
   */
  explicit SingleTimer(const std::string &name);
  /**
   * @brief The SingleTimer is linked to a TimerStatistics that
   * can take repeated measurements and log stats
   * add "SingleTimer _(__func__, timer);"
   * at the top of the function. If you have multiple functions with the same
   * name using the same TimerStatistics, you need to set the name manually
   * instead of using __func__. Otherwise the TimerStatistics will not be able
   * to distinguish between those two.
   *
   * @param name
   * @param timer
   */
  SingleTimer(const std::string &name, TimerStatistics *timer);
  /**
   * @brief Add the end of the function the SingleTimer
   * object goes out of scope and is thus destructed.
   * We use this to stop the timer.
   *
   */
  ~SingleTimer();
};

class Timer {
public:
  /**
   * Construct a new timer.
   * @param min_alpha Exponential decay, the higher the value the faster old
   *    timings fade out.
   */
  explicit Timer(double min_alpha = 0.01);

  /**
   * Cleanup the timer.
   */
  ~Timer() = default;

  /**
   * Start the timer.
   */
  void tick();

  /**
   * Stop the timer and return timer's string representation.
   */
  std::string tock();

  /**
   * Represent the timer's ema and emstd as string.
   *
   * Example: 0.0320 +-0.0002
   */
  std::string ToString();

protected:
  /**
   * Exponential moving avg/var/std decay.
   *
   * The higher this value the faster old timings' contributions fade out.
   */
  double min_alpha;

  /**
   * Epoch milliseconds of the last tick.
   */
  long start_ms = 0;

  /**
   * Exponential moving average.
   */
  double ema = 0.0;

  /**
   * Exponential moving variance.
   */
  double emvar = 0.0;

  /**
   * Exponential moving standard deviation.
   */
  double emstd = 0.0;

private:
  double alpha = 1.0;
};

} // namespace psi_act
