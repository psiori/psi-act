#pragma once
#include <memory>

#include "Logging/Logger.hpp"

namespace psi_act {

/**
 * @brief   Small wrapper class which saves a prefix, the log level and
 * identifier and defines operator<< to pipe stuff into, just like the LOG_
 * macros. Upon destruction, the buffered data is sent to the logger
 * It uses \ref log_stream() to do the actual logging.
 *
 */
struct LogWrapper {
  const Logger::Level level;
  const std::string prefix;
  const Logger::Identifier log_identifier;
  std::stringstream ss; ///< buffer for log items
  bool echo = false;    ///< echo to cout or not
  LogWrapper(const Logger::Level &level, const std::string &prefix,
             const Logger::Identifier &log_identifier, bool echo = false)
      : level{level}, prefix{prefix}, log_identifier{log_identifier},
        echo{echo} {
    ss << prefix << ": ";
  }

  LogWrapper(const LogWrapper &other) = delete; // Not sure what would happen
                                                // with the destructor behavior
                                                // when we allow copy

  template <typename T> LogWrapper &operator<<(const T &args) {
    ss << args;
    return *this;
  }

  ~LogWrapper() { log_stream(level, log_identifier, echo, ss); }
};

/**
 * @brief   Logging mixin allowing access to logging functions. It saves the
 * prefix and log identifier (overridable for each log invocation) to reuse it
 * everywhere.
 *
 * Usage:
 *
 * Inherit from this class and initialize it with your prefix of choice and
 * Logger::Identifier. You should use virtual inheritance if your class does or
 * will have derived classes, and these will then also be forced to initialize
 * the logging, it will not be done by the base class Constructor. You can also
 opt to
 * not inherit virtually and the subclass can then access the base class's
 logger
 *
 *      public class Wat : public virtual Loggable {
            public:
                Wat(Agent* agent) : Loggable("Wat " +
 agent->human_readable_id()) {}
 *      }
 *
 * Then, access the methods such as
 *
 *      error() << "This is an error";
 *
 * Or
 *
 *      warn(Logger::CONTROL_LOOP) << "Different identifier";
 *
 */
class Loggable {
protected:
  const std::string prefix;
  const Logger::Identifier log_identifier;

  LogWrapper always(const Logger::Identifier &log_identifier,
                    bool echo = false) const {
    return LogWrapper(Logger::Level::ALWAYS, prefix, log_identifier, echo);
  }

  LogWrapper always(bool echo = false) const {
    return always(log_identifier, echo);
  }

  LogWrapper error(const Logger::Identifier &log_identifier,
                   bool echo = false) const {
    return LogWrapper(Logger::Level::ERROR, prefix, log_identifier, echo);
  }

  LogWrapper error(bool echo = false) const {
    return error(log_identifier, echo);
  }

  LogWrapper warn(const Logger::Identifier &log_identifier,
                  bool echo = false) const {
    return LogWrapper(Logger::Level::WARN, prefix, log_identifier, echo);
  }

  LogWrapper warn(bool echo = false) const {
    return warn(log_identifier, echo);
  }

  LogWrapper debug(const Logger::Identifier &log_identifier,
                   bool echo = false) const {
    return LogWrapper(Logger::Level::DEBUG, prefix, log_identifier, echo);
  }

  LogWrapper debug(bool echo = false) const {
    return debug(log_identifier, echo);
  }

  LogWrapper info(const Logger::Identifier &log_identifier,
                  bool echo = false) const {
    return LogWrapper(Logger::Level::INFO, prefix, log_identifier, echo);
  }

  LogWrapper info(bool echo = false) const {
    return info(log_identifier, echo);
  }

  LogWrapper trace(const Logger::Identifier &log_identifier,
                   bool echo = false) const {
    return LogWrapper(Logger::Level::TRACE, prefix, log_identifier, echo);
  }

  LogWrapper trace(bool echo = false) const {
    return trace(log_identifier, echo);
  }

  // TODO: delete, there are no fatal logs, per definition
  LogWrapper fatal(const Logger::Identifier &log_identifier,
                   bool echo = false) const {
    return LogWrapper(Logger::Level::FATAL, prefix, log_identifier, echo);
  }

  LogWrapper fatal(bool echo = false) const {
    return fatal(log_identifier, echo);
  }

public:
  /**
   * @brief Constructor
   *
   * @param prefix  Prefix to prepend to each log line.
   * @param identifier Identifier to use by default.
   */
  Loggable(const std::string &prefix, const Logger::Identifier &identifier)
      : prefix{prefix}, log_identifier{identifier} {}
};
} // namespace psi_act
