#pragma once

/**
 * @file TimeUtils.hpp
 *
 * \note If not stated otherwise, the methods in here will produce / use UTC.
 */

#include <psiact/TimePoint.h>
#include <string>

#define TICK() utils::millis_since_epoch(psi_act::Clock::now())
#define TOCK(tick) utils::millis_since_epoch(psi_act::Clock::now()) - (tick);

namespace psi_act {

/**
 * @brief Inequality operator for std::tm. Uses only sec, min, hour, mday, mon
 * and year fields
 *
 * @param t1
 * @param t2
 *
 * @return If calendar dates are identical up to the ignored fields
 */
bool operator==(const std::tm &t1, const std::tm &t2);

namespace utils {

/**
 * @brief   Converts TimePoint to Time
 * @param   t   Time point to convert (relative to `psi_act::Clock`)
 * @return  Time struct
 */
std::tm _tm_from_tp(const psi_act::TimePoint &t);

/**
 * @brief Convert ms since epoch to a TimePoint.
 * @param ms ms since epoch 1970
 * @returns TimePoint
 */
psi_act::TimePoint ms2tp(long ms);

/**
 * @brief   Format a time point as "%Y-%m-%dT%H:%M:%S"
 * This follows the ISO8601 format described here:
 * https://en.wikipedia.org/wiki/ISO_8601
 * @param   t   Time point to convert (relative to `psi_act::Clock`)
 * @return  Time formatted as string
 */
std::string format_timepoint_iso8601(const psi_act::TimePoint &t,
                                     bool add_zone = true,
                                     bool add_millis = true);

/**
 * @brief   Format a time point as "<year>/<month>/<day>" with decimal numbers
 * @return  Date string
 */
std::string format_timepoint_date_only(const psi_act::TimePoint &t);

std::string format_timepoint_time_only(const psi_act::TimePoint &t,
                                       bool add_millis = true);

/**
 * @brief Format milliseconds as ``0HH:MM:SS.0MS``.
 */
#pragma GCC diagnostic ignored "-Wunused-function"
std::string format_ms(const std::chrono::milliseconds &ms);

/**
 * @brief Format timepoint as ``HH:MM:SS``.
 */
#pragma GCC diagnostic ignored "-Wunused-function"
std::string format_daytime(const psi_act::TimePoint &timepoint);

#pragma GCC diagnostic ignored "-Wunused-function"
/* returns UTC date in the form of YY-MM-DD. */
std::string format_date(const psi_act::TimePoint &timepoint);

/**
 * Removes ":." from string and replaces them with "__".
 */
std::string sanitize_for_filename(const std::string &str);

/*
 * Create a TimePoint object (which is expressed in system time) from an
 * ISO8601 timestamp (which is expressed in UTC), the inverse of
 * ``format_timepoint_iso8601(..., true, true)``.
 *
 * Example input: ``2021-07-14T10:08:01.937Z``
 *
 * @param sanitized_for_filename If given, the parsed ISO8601 timestamp string
 *        is expected to have had all periods and colons replaced by
 *        underscores, making this method the inverse of
 *        ``sanitize_for_filename(format_timepoint_iso8601(..., true, true))``.
 *        Example input if this parameter is set to true:
 *        ``2021-07-14T10_08_01_937Z``
 *
 * @note Works only on POSIX systems.
 */
psi_act::TimePoint parse_timepoint_iso8601(const std::string &formatted_time,
                                           bool sanitized_for_filename = false);

/**
 * @brief   Get a duration object (seconds resolution) from a string of the
 * form "HH:MM:SS". The parse is successful if at least one part is present,
 * so anything matching "[[HH:]MM:]SS" should work.
 * @return  Duration
 * @throws  std::invalid_argument if the argument cannot be parsed into
 * seconds
 */
psi_act::Duration durationFromHHMMSS(const std::string &duration_spec);

/**
 * @brief Convert a timepoint to a "milliseconds since unix epoch" integer.
 * @param timepoint  TimePoint to convert.
 * @returns          Milliseconds passed since unix epoch.
 */
long millis_since_epoch(const psi_act::TimePoint &timepoint);

/**
 * @brief Convert a duration to milliseconds.
 * @param timepoint  Duration to convert.
 * @returns          Milliseconds.
 */
long to_ms(const psi_act::Duration &duration);

/**
 * @brief Convert a duration to microseconds.
 * @param timepoint  Duration to convert.
 * @returns          Milliseconds.
 */
long to_micros(const psi_act::Duration &timepoint);

/**
 * @brief Convert a duration to nanoseconds.
 * @param timepoint  Duration to convert.
 * @returns          nanos.
 */
long to_ns(const psi_act::Duration &duration);

/**
 * @brief Convert timepoint to seconds since the epoch.
 * @param t Timepoint
 * @return Number of seconds since the epoch until the timepoint
 */
long seconds_since_epoch(psi_act::TimePoint t);

/**
 * @brief Convert a duration to seconds.
 * @param timepoint  Duration to convert.
 * @returns          seconds.
 */
long to_s(const psi_act::Duration &duration);

/**
 * @brief returns duration in at least microsecond precision
 *
 * @tparam T type of return value
 * @param duration duration that shall be converted to seconds
 * @return T duration in seconds
 */
template <typename T> T to_seconds(const psi_act::Duration &duration) {
  return std::chrono::duration_cast<std::chrono::duration<T>>(duration).count();
}

/** returns a duration in seconds with millisecond precision. */
/**
 * @todo    move impl to cpp
 */
inline double durationInSeconds(const psi_act::TimePoint &start,
                                const psi_act::TimePoint &end) {
  return to_ms(end - start) / 1000.0;
}

/**
 * @todo    Documentation missing
 * @todo    Move impl to cpp
 */
inline double normalizeToSecond(double value, const psi_act::TimePoint &start,
                                const psi_act::TimePoint &end,
                                double delta_min = 0.001) {
  double dt = utils::durationInSeconds(start, end);
  return dt >= delta_min ? value / delta_min : 0.0;
}

} // namespace utils

} // namespace psi_act
