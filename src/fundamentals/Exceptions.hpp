#pragma once
#include <sstream>
#include <stdexcept>
#include <string>

namespace psi_act {
namespace exceptions {
class BaseException : public std::exception {
private:
  std::string msg_;

public:
  explicit BaseException(const std::string &msg) : msg_(msg) {}

  const char *what() const noexcept override { return msg_.c_str(); }
};

class IOException : public std::runtime_error {
public:
  explicit IOException(const std::string &msg) : std::runtime_error(msg) {}
};

/**
 * @class BadFile
 * @brief Exception for use when file handling fails
 */
class BadFile : public BaseException {
public:
  explicit BadFile(const std::string &msg) : BaseException(msg) {}
};
/**
 * @class ConfigError
 * @brief Thrown on bad configurations / config files.
 */
class ConfigError : public BaseException {
public:
  explicit ConfigError(const std::string &msg) : BaseException(msg) {}
};

template <typename T> class BadConversion : public std::invalid_argument {
public:
  explicit BadConversion(const std::string &msg) : std::invalid_argument(msg) {}
};
} // namespace exceptions
} // namespace psi_act