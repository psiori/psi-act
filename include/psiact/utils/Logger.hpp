// ///////////////////////////////////////////////////////////////////////////
//
//   AUTOCRANE
//
//   PSIORI GmbH -- all rights reserved.
//
// ///////////////////////////////////////////////////////////////////////////

#ifndef AUTOCRANE_LOGGER_HPP
#define AUTOCRANE_LOGGER_HPP

#include <boost/circular_buffer.hpp>
#include <boost/thread/sync_bounded_queue.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <psiact/TimePoint.h>
#include <sstream>
#include <string>
#include <thread>

namespace sfs = std::filesystem;

namespace psi_act {

/**
 * @class Logger
 * @brief The logger class is responsible for logging data pertaining to
 *      the world model, sensor, command, decision making, control loop,
 *      behaviour, time and the crane simulator in various levels.
 *
 * Do not access the logger directly but use the macros at the end
 * of the file including LOG_DEBUG, LOG_ERROR, LOG_WARN, LOG_INFO etc.
 *
 */
class Logger {
private:
  /** The singleton logger. */
  static std::unique_ptr<Logger> instance;

  /** Whether logging is disabled completely. */
  bool disabled;

public:
  /**
   *  @enum Level
   *  @brief Different logging levels
   */
  enum Level {
    TRACE = 0, /* For trace messages */
    DEBUG,     /* For debugging purposes */
    INFO,      /* For relevant information */
    WARN,      /* All possible warnings */
    ERROR,     /* For any sorts of errors */
    FATAL,     /* Fatal errors */
    ALWAYS,    /* messages which are not errors, but have highest priority and
                  should never be dropped */
    _LAST_LEVEL
  };

  static std::array<const char *, Level::_LAST_LEVEL> Level2String;

  /**
   *  @enum Identifier
   *  @brief Identifies the object sending a log message
   */
  enum Identifier {
    SENSOR,          /* message from sensors */
    PROCESSING,      /* message from sensor processing or fusion */
    BEHAVIOUR,       /* message from behaviour */
    CONTROL_LOOP,    /* message from control loop */
    WORLD_MODEL,     /* message from world model */
    CRANE,           /* message from crane odometry */
    COMMAND,         /* message from command/drive vector */
    DECISION_MAKING, /* message from decision making */
    WORLD_OBSERVER,  /* message for observer */
    CONFIGURATION,   /* message from configuration */
    SAFETY,          /* message from safety overseer */
    SETUP,           /* message from setup and system start */
    SHUTDOWN,        /* message from system shutdown */
    LOGGING,         /* message for logging activities */
    POSTPROCESSING,  /* message for post-processing  */
    SENSOR_MANAGER,  /* message for sensor manager */
    _LAST_IDENTIFIER
  };

  static std::array<const char *, Identifier::_LAST_IDENTIFIER>
      Identifier2String;

  enum RotationTimeout { NONE, MINUTELY, HOURLY, DAILY, MONTHLY, YEARLY };

  static RotationTimeout parseRotationTimeout(const std::string &rotation);

  struct PriorityLog {
    psi_act::TimePoint timestamp;
    long cycle;
    std::string log;
  };

  static std::map<Level, boost::circular_buffer<PriorityLog>>
      recent_high_prio_logs;

  static std::map<Level, long> high_prio_counts;

  struct LogItem {
    long counter;
    psi_act::TimePoint date_time;
    psi_act::TimePoint started_at;
    psi_act::TimePoint desired_end_at;
    Logger::Level level;
    Logger::Identifier id;
    std::string message;
  };

  explicit Logger(Level level = Level(0), bool use_thread_guard = true,
                  bool flush_always = true, const std::string &suffix = "");

  /**
   *  @brief Close the output stream file
   */
  ~Logger();

  /**
   * returns true if the priority of the argument level is at
   * least as large as the present log-level. Use to wrap complex
   * calculations done just for creating the log output so it won't
   * be calculated if it won't be written out.
   *
   * Example:
   * if (LOG.level(Logger::DEBUG)) {
   *   std::string message = complex_calculations_for_debug_output();
   *   LOG_DEBUG(Logger::SENSOR, message);
   * }
   */
  bool level(Level level) { return level >= log_level_; }

  /**
   * @brief Thread-safe logging function.
   * @param level Level of logging like debug, info, warn, fatal etc.
   * @param id Identifier to identify the component to which message belongs
   * @param message What to log (data).
   * @note   Threadsafe when use_thread_guard is true
   */
  std::ostream &log(Level level, Identifier id, const std::string &message);

  /**
   * @brief set the log level
   * @warning   Not threadsafe.
   */
  void setLogLevel(const std::string &level);

  /**
   * @brief set flush behavior to either flush output stream (true) or not
   * (false) for every invocation
   * @warning   Not threadsafe.
   */
  void setFlush(bool flush) { flush_always_ = flush; }

  /**
   * @brief set whether or not to use a mutex to guard the log method
   * @warning   Not threadsafe.
   */
  void setThreadGuard(bool guard) { use_thread_guard_ = guard; }

  /**
   * @brief Initialize the log file paths.
   * Cannot be in constructor as the filename of the primary logfile is
   * suffixed for sensornode processes in order to not collide with the
   * core process when writing to the same directory.
   * @param suffix Optional suffix to add to log files.
   */
  void initLogFilePaths(const std::string &suffix = "");

  /**
   * @brief Initialize the singleton logger instance explicitly
   * @param suffic Optional suffix to add to log files.
   */
  static void init(const std::string &suffix = "");

  /**
   * @return The singleton Logger instance.
   */
  static Logger &getLogger();

  /**
   * @brief Close the current log file, rename it and open a new one. The naming
   * scheme is "logfile.txt -> logfile.txt.1 -> logfile.txt.2 ...". Appending
   * the files with suffix in the order of their suffix gives the log file
   * before the last rotation. "logfile.txt" always contains the latest chunk
   * @param create_new  Enable/disable opening of new file (in case you are done
   * and don't want to leave empty files behind)
   */
  void rotateLogFile(bool create_new, std::tm &date, RotationTimeout &timeout,
                     std::ofstream &stream, sfs::path &file_path);

  /** Thread writing queue to disk in separate thread. */
  void work_on_queue();

  /**
   * @brief Returns log queue size of thread worker.
   */
  int get_queue_size() const;

protected:
  /** Log level to log at. Logs with lower severity will be ignored. */
  Level log_level_;

  /**
   * @brief Get the logging date and time stamp
   * @return date and time stamp
   */
  static std::tm getLogDate(const RotationTimeout &rotation_timeout);

  /** The output file stream to write the logs in some path. */
  std::ofstream file_stream_;

  sfs::path file_path_; ///< name of the output file

  /** The backup output file stream to write the logs in some path. */
  std::ofstream backup_file_stream_;

  size_t backup_file_count_; ///< Number of backup files to keep

  sfs::path backup_file_path_; ///< Name of the backup output file.

  std::mutex log_mutex_; ///< Mutual exclusive access to log streams.

  bool use_thread_guard_; ///< Make logging threadsafe.

  bool flush_always_; ///< enable immediate write without buffering

  /** How often to rotate the standard logs. **/
  RotationTimeout rotation_timeout_;

  /** How often to rotate the backup logs. **/
  RotationTimeout backup_rotation_timeout_;

  std::tm log_date_; ///< Date of logging the standard logs.

  std::tm backup_log_date_; ///< Date of logging the backup logs.

  typedef boost::sync_bounded_queue<LogItem> Queue;
  Queue queue_;        ///< Queue containing log entries
  std::thread worker_; ///< worker thread writing logs until `stopped_`
  bool stopped_;       ///< stop work on the thread
};
} // namespace psi_act

#define LOGGER (psi_act::Logger::getLogger())

// kinda ugly: The cmake build defines the Macro which must be one of
// Logger::Level. If they somehow change it'll break
#ifndef MIN_LOG_LEVEL
#define MIN_LOG_LEVEL Logger::Level(0)
#endif

#ifndef DISABLE_LOGGING
namespace psi_act {
template <typename... T>
static void log_stream(const Logger::Level &level, const Logger::Identifier &id,
                       bool echo, const T &...args) {
  if (level >= Logger::MIN_LOG_LEVEL) {
    std::stringstream _ss;
    _ss << (... << args);
    const auto msg = _ss.str();
    if (echo) {
      std::cout << msg << std::endl;
    }
    Logger::getLogger().log(level, id, msg);
  }
}

template <
    typename T,
    std::enable_if_t<std::is_same<T, std::stringstream>::value, bool> = false>
static void log_stream(const Logger::Level &level, const Logger::Identifier &id,
                       bool echo, const T &ss) {
  if (level >= Logger::MIN_LOG_LEVEL) {
    const auto msg = ss.str();
    if (echo) {
      std::cout << msg << std::endl;
    }
    Logger::getLogger().log(level, id, msg);
  }
}
} // namespace psi_act
#else
namespace psi_act {
template <typename... T>
static void log_stream(const Logger::Level &level, const Logger::Identifier &id,
                       bool echo, const T &...args) {
  // do nothing
}
} // namespace psi_act
#endif

// <------  CAUTION, macro magic here, avert your eyes  ---------->
// The following shenanigans are necessary to have a 3 and 4 arg version of the
// same macro. So you can use LOG_STREAM(FATAL, SETUP, "foobar") as well as
// LOG_STREAM(FATAL, SETUP, "foobar", true) for echoing to stdout as well.
// The downside is that LOG_STREAM(FATAL, SETUP, "foobar", false) is valid, but
// useless, as you could just use the first version
#define LOG_STREAM_NO_ECHO(level, identifier, args)                            \
  {                                                                            \
    if (level >= psi_act::Logger::MIN_LOG_LEVEL) {                             \
      std::stringstream _ss;                                                   \
      _ss << args;                                                             \
      const auto msg = _ss.str();                                              \
      psi_act::Logger::getLogger().log(level, identifier, msg);                \
    }                                                                          \
  }

#define LOG_STREAM_ECHO(level, identifier, args, echo)                         \
  {                                                                            \
    if (level >= psi_act::Logger::MIN_LOG_LEVEL) {                             \
      std::stringstream _ss;                                                   \
      _ss << args;                                                             \
      const auto msg = _ss.str();                                              \
      psi_act::Logger::getLogger().log(level, identifier, msg);                \
      bool do_echo = echo;                                                     \
      if (do_echo) {                                                           \
        std::cout << msg << std::endl;                                         \
      }                                                                        \
    }                                                                          \
  }

#define LOG_STREAM_INVALID(...)                                                \
  static_assert(false, "LOG_STREAM macro exists for 3 and 4 arguments only");
#define GET_MACRO(_1, _2, _3, _4, NAME, ...) NAME

// <------  end macro magic  ---------->

/*
 * @brief   Log a message in the logger, optionally printing to cout as well.
 *
 * @param   level   Minimum necessary log level to display the message
 * @param   identifier  Logger::Identifier for the message
 * @param   args    Argument string, all contents will be piped as-is into a
 * stringstream
 * @param   echo    Boolean denoting whether message should be echoed to stdout
 * as well. If you leave it out, no echoing will be performed.
 *
 * Convenience macro to reduce typing are provided below
 *
 * Example: LOG_DEBUG(Logger::SENSOR, "Found camera with id: " <<
 * device_id);
 */
#define LOG_STREAM(...)                                                        \
  GET_MACRO(__VA_ARGS__, LOG_STREAM_ECHO, LOG_STREAM_NO_ECHO,                  \
            LOG_STREAM_INVALID)                                                \
  (__VA_ARGS__)

// If logging is disabled, only noop macros are provided. By default, logging
// is enabled.
#ifndef DISABLE_LOGGING

#define LOG_ALWAYS(...) LOG_STREAM(psi_act::Logger::ALWAYS, __VA_ARGS__)
#define LOG_FATAL(...) LOG_STREAM(psi_act::Logger::FATAL, __VA_ARGS__)
#define LOG_ERROR(...) LOG_STREAM(psi_act::Logger::ERROR, __VA_ARGS__)
#define LOG_WARN(...) LOG_STREAM(psi_act::Logger::WARN, __VA_ARGS__)
#define LOG_INFO(...) LOG_STREAM(psi_act::Logger::INFO, __VA_ARGS__)
#define LOG_DEBUG(...) LOG_STREAM(psi_act::Logger::DEBUG, __VA_ARGS__)
#define LOG_TRACE(...) LOG_STREAM(psi_act::Logger::TRACE, __VA_ARGS__)

#else // ifdef DISABLE_LOGGING

#define LOG_ALWAYS(...)
#define LOG_FATAL(...)
#define LOG_ERROR(...)
#define LOG_WARN(...)
#define LOG_INFO(...)
#define LOG_DEBUG(...)
#define LOG_TRACE(...)

#endif // ifndef DISABLE_LOGGING

#endif /* AUTOCRANE_LOGGER_HPP */
