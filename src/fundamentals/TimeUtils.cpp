#include "psiact/fundamentals/TimeUtils.hpp"

// different header on windows and posix
#include <ctime>

#if defined(_WIN32) && _WIN32
// windows has equivalent function like posix timegm
#define timegm _mkgmtime

/**
 * @brief   Surrogate gmtime_r function since windows does not have it, but
 * supports partially the same time types like posix time.h header
 *
 * convert a timestamp into calendar time (utc)
 *
 * @param timep unix epoch timestamp
 * @param result calendar time
 *
 * @return \ref result
 */
struct tm *gmtime_r(const time_t *timep, struct tm *result) {
  struct tm *ret = gmtime(timep);
  if (!ret) {
    return nullptr;
  } else {
    *result = *ret;
    return ret;
  }
}
#endif

#include <iomanip>
#include <regex>
#include <sstream>

namespace psi_act {

bool operator==(const std::tm &t1, const std::tm &t2) {
  return t1.tm_sec == t2.tm_sec && t1.tm_min == t2.tm_min &&
         t1.tm_hour == t2.tm_hour && t1.tm_mday == t2.tm_mday &&
         t1.tm_mon == t2.tm_mon && t1.tm_year == t2.tm_year;
}

namespace utils {

psi_act::TimePoint ms2tp(long ms) {
  std::chrono::milliseconds duration_ms(ms);
  return psi_act::TimePoint() +
         std::chrono::duration_cast<psi_act::Duration>(duration_ms);
}

std::tm _tm_from_tp(const psi_act::TimePoint &t) {
  std::tm calendar_time_utc{};
  const std::time_t as_time_t = psi_act::Clock::to_time_t(t);
  auto *return_value = gmtime_r(&as_time_t, &calendar_time_utc);
  if (!return_value) {
    throw std::runtime_error(
        "Could not convert TimePoint to UTC calendar time.");
  } else {
    return *return_value;
  }
}

static int _get_millis_from_tp(const psi_act::TimePoint &t) {
  auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
      t.time_since_epoch());
  return duration_ms.count() % 1000;
}

std::string format_timepoint_iso8601(const psi_act::TimePoint &t, bool add_zone,
                                     bool add_millis) {
  const std::tm tm = _tm_from_tp(t);

  std::stringstream ss;
  ss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");
  if (add_millis) {
    ss << "." << std::setw(3) << std::setfill('0') << _get_millis_from_tp(t);
  }
  if (add_zone) {
    ss << "Z";
  }
  return ss.str();
}

std::string format_timepoint_date_only(const psi_act::TimePoint &t) {
  std::tm tm = _tm_from_tp(t);
  std::stringstream ss;
  ss << std::put_time(&tm, "%Y-%m-%d");
  return ss.str();
}

std::string format_timepoint_time_only(const psi_act::TimePoint &t,
                                       bool add_millis) {
  const std::tm tm = _tm_from_tp(t);

  std::stringstream ss;
  ss << std::put_time(&tm, "%H:%M:%S");

  if (add_millis) {
    ss << "." << std::setw(3) << std::setfill('0') << _get_millis_from_tp(t);
  }

  return ss.str();
}

std::string sanitize_for_filename(const std::string &str) {
  static const std::regex r("[:\\.]", std::regex_constants::optimize);
  return std::regex_replace(str, r, "_");
}

psi_act::TimePoint parse_timepoint_iso8601(const std::string &formatted_time,
                                           bool sanitized_for_filename) {
  // Note: This method uses both a regex and sscanf. This is suboptimal.. One
  // might want to switch to regex only at some point?
  static const std::regex iso8601_sanitized = std::regex{
      R"([[:digit:]]{4}-[[:digit:]]{2}-[[:digit:]]{2})"
      R"(T[[:digit:]]{2}_[[:digit:]]{2}_[[:digit:]]{2}_[[:digit:]]{3}Z)",
      // differences:   ^              ^              ^
      std::regex::egrep | std::regex::optimize};
  static const std::regex iso8601_strict = std::regex{
      R"([[:digit:]]{4}-[[:digit:]]{2}-[[:digit:]]{2})"
      R"(T[[:digit:]]{2}:[[:digit:]]{2}:[[:digit:]]{2}\.[[:digit:]]{3}Z)",
      // differences:   ^              ^              ^^
      std::regex::egrep | std::regex::optimize};
  const std::regex &iso8601_format =
      sanitized_for_filename ? iso8601_sanitized : iso8601_strict;
  if (!std::regex_match(formatted_time, iso8601_format)) {
    const std::string example = sanitized_for_filename
                                    ? "2019-09-28T21_43_34_672Z"
                                    : "2019-09-28T21:43:34.672Z";
    throw std::invalid_argument(
        "'" + formatted_time +
        "' is not a valid formatted time (expected format '" + example + "')");
  }
  unsigned int year, month, day, hour, minute, second, millisecond;
  std::sscanf(formatted_time.c_str(),
              sanitized_for_filename ? "%u-%u-%uT%u_%u_%u_%3uZ"
                                     : "%u-%u-%uT%u:%u:%u.%3uZ",
              &year, &month, &day, &hour, &minute, &second, &millisecond);
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
  std::tm c_time_utc{0};
  c_time_utc.tm_sec = second;
  c_time_utc.tm_min = minute;
  c_time_utc.tm_hour = hour;
  c_time_utc.tm_mday = day;
  c_time_utc.tm_mon = month - 1;
  c_time_utc.tm_year = year - 1900; // tm uses years relative to 1900
  return std::chrono::system_clock::from_time_t(timegm(&c_time_utc)) +
         std::chrono::milliseconds(millisecond);
}

psi_act::Duration durationFromHHMMSS(const std::string &duration_spec) {
  // Timeouts can be given in the format "HH[:MM[:SS]]"
  unsigned int hours = 0, minutes = 0, seconds = 0;
  int scanf_result = std::sscanf(duration_spec.c_str(), "%2u:%2u:%2u", &hours,
                                 &minutes, &seconds);
  std::chrono::seconds duration = std::chrono::hours(hours) +
                                  std::chrono::minutes(minutes) +
                                  std::chrono::seconds(seconds);
  if (scanf_result < 1) {
    throw std::invalid_argument(std::string("Could not convert ") +
                                duration_spec + " into at duration.");
  }
  return duration;
}

long millis_since_epoch(const psi_act::TimePoint &timepoint) {
  return std::chrono::time_point_cast<std::chrono::milliseconds>(timepoint)
      .time_since_epoch()
      .count();
}

long to_ns(const psi_act::Duration &duration) {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
}

long to_ms(const psi_act::Duration &duration) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(duration)
      .count();
}

long to_micros(const psi_act::Duration &duration) {
  return std::chrono::duration_cast<std::chrono::microseconds>(duration)
      .count();
}

long seconds_since_epoch(psi_act::TimePoint t) {
  return to_s(t.time_since_epoch());
}

long to_s(const psi_act::Duration &duration) {
  return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
}

std::string format_ms(const std::chrono::milliseconds &ms) {
  constexpr size_t len = 100;
  char buf[len];

  int millis = ms.count() % 1000;
  int seconds =
      std::chrono::duration_cast<std::chrono::seconds>(ms).count() % 60;
  int minutes =
      std::chrono::duration_cast<std::chrono::minutes>(ms).count() % 60;
  long hours = std::chrono::duration_cast<std::chrono::hours>(ms).count();

  double s = seconds + (millis / 1000.);

  std::snprintf(buf, len, "%03ld:%02d:%06.3lf", hours, minutes, s);
  return std::string(buf);
}

std::string format_daytime(const psi_act::TimePoint &timepoint) {
  auto timer = std::chrono::system_clock::to_time_t(timepoint);
  std::tm bt = *std::gmtime(&timer);
  std::ostringstream oss;
  oss << std::put_time(&bt, "%H:%M:%SZ"); // HH:MM:SSZ - UTC Zulu
  return oss.str();
}

std::string format_date(const psi_act::TimePoint &timepoint) {
  auto timer = std::chrono::system_clock::to_time_t(timepoint);
  std::tm bt = *std::gmtime(&timer);
  std::ostringstream oss;
  oss << std::put_time(&bt, "%y-%m-%d"); // YY-MM-DD
  return oss.str();
}

} // namespace utils

} // namespace psi_act
