// ///////////////////////////////////////////////////////////////////////////
//
//   psi_act
//
//   PSIORI GmbH -- all rights reserved.
//
// ///////////////////////////////////////////////////////////////////////////

#include "psiact/utils/Logger.hpp"

#include <iomanip>
#include <psiact/CycleManager.h>
#include <string>

#include "psiact/fundamentals/Exceptions.hpp"
#include "psiact/fundamentals/ThreadUtils.hpp"
#include "psiact/fundamentals/TimeUtils.hpp"
#include "psiact/fundamentals/Utils.hpp"
#include "psiact/utils/YAMLReader.hpp"

// rotation     |   overwrite
// true         |   true        A
// true         |   false       B
// false        |   true        C
// false        |   false       D
//
// A: Simple, just use rotation naming scheme and overwrite
// B: For initial file, make unique, for rotated logfiles, overwrite,
// otherwise it gets too complicated (when initial file from previous run
// was deleted, but rotated ones remain) C: Simple, use just base name and
// overwrite D: Simple, use just base name and make unique

using namespace std::literals::chrono_literals;

namespace psi_act {

/**
 * @brief Create file stream amd respective paths.
 * @param filepath Path to stream to.
 * @param overwrite Whether to overwrite an existing file at the path. If set to
 *                  false, the existing file will be moved away with an
 *                  incremental suffix.
 * @rparam disabled_overwrite Whether overwrite was disabled due to the target
 *                            locations nature (e.g. in the data upload
 *                            directory).
 * @rparam moved_existing Whether and where an existing file was moved.
 * @return Output file stream
 */
static std::ofstream initLogFile(const sfs::path &filepath, bool overwrite,
                                 bool &disabled_overwrite,
                                 sfs::path &moved_existing) {
  // Only allow overwriting logs when they are not shared with others (aka
  // destined to be uploaded to the cloud).
  const bool shared_log =
      filepath.string().find("/home/autocrane/recordings") == 0;
  if (shared_log && overwrite) {
    overwrite = false;
    // Still in setup, cannot log yet.
    disabled_overwrite = true;
  }

  // If specified log file exists, copy content to new unique log file (unique
  // by numbering) and remove found log file.
  if (!overwrite && utils::file::exists(filepath)) {
    for (unsigned int suffix_num = 1;; ++suffix_num) {
      // I guess we don't need to check for overflow here, right?
      const sfs::path candidate =
          filepath.parent_path() /
          sfs::path(filepath.stem().string() + "_" +
                    std::to_string(suffix_num) + filepath.extension().string());
      if (!utils::file::exists(candidate)) {
        utils::file::copy(filepath, candidate);
        utils::file::remove(filepath);
        // Still in setup, cannot log yet.
        moved_existing = candidate;
        break;
      }
    }
  }

  // If log file path has a non-existent parent directory, try to create it.
  if (!filepath.empty() && filepath.has_parent_path() &&
      !utils::file::exists(filepath.parent_path()) &&
      !utils::file::create_directories(filepath.parent_path())) {
    throw std::runtime_error(
        std::string("Could not create log file parent directory ") +
        filepath.string() + filepath.parent_path().string());
  }

  auto file_stream = std::ofstream(filepath.string(),
                                   std::ofstream::out | std::ofstream::trunc);
  if (!file_stream.good()) {
    throw std::runtime_error(
        std::string("Could not open log file stream to file ") +
        filepath.string());
  }
  file_stream << std::left;
  return file_stream;
}
static std::ofstream initLogFile(const sfs::path &filepath, bool overwrite) {
  bool disabled_overwrite;
  sfs::path moved_existing;
  return initLogFile(filepath, overwrite, disabled_overwrite, moved_existing);
}

/**
 * @brief Write log entry to disk.
 * If configured:
 * - flushes
 * - write log also to backup stream
 */
static std::ostream &log(std::ostream &file_stream,
                         std::ostream &file_backup_stream, bool flush_always,
                         Logger::Level level, Logger::Identifier id,
                         const std::string &message);

Logger::Logger(Level level, bool use_thread_guard, bool flush_always,
               const std::string &suffix)
    : log_level_{level}, use_thread_guard_{use_thread_guard},
      flush_always_{flush_always},
      // Queue size larger than expected, as it needs to handle spikes caused by
      // os file flushing.
      queue_{AC_CR.get<unsigned int>(
          "logger.queue_size",
          static_cast<int>(2 /* s */ * CM.getFrequency() *
                           380 /* logs_each_cycle */),
          false)},
      stopped_{false} {
  /*****************************************************************************
   *  ATTENTION: When accessing config here, always pass log_missing_value=false
   *  otherwise you'll likely create a deadlock  *
   ****************************************************************************/
  auto conf = AC_CR.get("logger", ConfigNode::Map(), false);

  this->disabled = conf.get<bool>("disabled", false, false);
  rotation_timeout_ = Logger::RotationTimeout::NONE;

  if (!disabled) {
    rotation_timeout_ = psi_act::Logger::parseRotationTimeout(
        conf.get<std::string>("rotation", "NONE", false));
    backup_rotation_timeout_ = rotation_timeout_;
    log_date_ = psi_act::Logger::getLogDate(rotation_timeout_);
    file_path_ =
        sfs::path(conf.get<std::string>("log_file_name", "psi_act.txt", false));
    if (conf.contains("backup")) {
      const auto &backup_conf = conf.get("backup");
      backup_rotation_timeout_ = psi_act::Logger::parseRotationTimeout(
          backup_conf.get<std::string>("rotation", "NONE", false));
      backup_log_date_ = psi_act::Logger::getLogDate(backup_rotation_timeout_);
      backup_file_count_ = backup_conf.get<size_t>("num_keep", 1, false);
      backup_file_path_ =
          sfs::path(backup_conf.get<std::string>("file_name", "", false));
    }
    this->initLogFilePaths(suffix);
    this->log(ALWAYS, Logger::LOGGING,
              std::string("Initial log level is ") + Level2String[level]);
  }
  worker_ = std::thread(&Logger::work_on_queue, this);
}

void Logger::initLogFilePaths(const std::string &suffix) {
#ifdef DISABLE_LOGGING
  // If logging is disabled, do not create a log file.
  return;
#endif
  /*****************************************************************************
   *  ATTENTION: When accessing config here, always pass log_missing_value=false
   *  otherwise you'll likely create a deadlock  *
   ****************************************************************************/

  const ConfigNode &conf = AC_CR.get("logger", ConfigNode::Map(), false);

  // Add crane/trolley suffix to both the log and the backup log path.
  if (!suffix.empty()) {
    file_path_ =
        file_path_.parent_path() / (file_path_.stem().string() + "-" + suffix +
                                    file_path_.extension().string());
    if (!backup_file_path_.empty()) {
      backup_file_path_ = backup_file_path_.parent_path() /
                          (backup_file_path_.stem().string() + "-" + suffix +
                           backup_file_path_.extension().string());
    }
  }

  // Add a suffix for sensornode logs so one sensornode can run on the same
  // machine as one core process. This does not solve the "many sensornodes
  // on one box" problem.
  if (AC_CR.isSensorNode()) {
    file_path_ =
        file_path_.parent_path() / (file_path_.stem().string() + "_sensornode" +
                                    file_path_.extension().string());
  }

  // Finally actually setup the log file.
  bool disabled_overwrite = false;
  sfs::path moved_existing;
  bool overwrite = conf.get<bool>("overwrite", false, false);
  file_stream_ =
      initLogFile(file_path_, overwrite, disabled_overwrite, moved_existing);

  // Setup the backup log file if desired.
  if (!backup_file_path_.empty()) {
    bool backup_overwrite = conf.get<bool>("backup.overwrite", true, false);
    backup_file_stream_ = initLogFile(backup_file_path_, backup_overwrite);
  }

  // Done setup, start logging.
  this->log(INFO, Logger::LOGGING,
            std::string("Logging to ") + file_path_.string());
  if (!backup_file_path_.empty()) {
    this->log(INFO, Logger::LOGGING,
              std::string("Backup logging to ") + backup_file_path_.string());
  }
  if (disabled_overwrite) {
    this->log(
        INFO, Logger::LOGGING,
        "Not overwriting log file as it is placed into a shared directory.");
  }
  if (!moved_existing.empty()) {
    this->log(INFO, Logger::LOGGING,
              std::string("Moved existing logfile from ") +
                  file_path_.string() + " to " + moved_existing.string() +
                  " to avoid overwriting it.");
  }
}

void Logger::rotateLogFile(bool create_new, std::tm &date,
                           RotationTimeout &timeout, std::ofstream &stream,
                           sfs::path &file_path) {
  // Only call this if you hold the lock already. That also means you can not
  // call 'this->log' from this method as that would want to lock again, instead
  // use psi_act::log as its already used below.

  // Is there even a file to rotate?
  if (!utils::file::exists(file_path)) {
    psi_act::log(file_stream_, backup_file_stream_, false, Logger::INFO,
                 Logger::LOGGING,
                 std::string("Not rotating ") + file_path.string() +
                     " as it does not exist.");
    if (create_new) {
      stream = std::ofstream(file_path.string(),
                             std::ofstream::out | std::ofstream::trunc);
      stream << std::left;
      date = Logger::getLogDate(timeout);
    }
    return;
  }

  // This can happen when terminating the process.
  // We need to use the exact timestamp, as otherwise we would end up in
  // duplicate file names for our rotated log files.
  if (date == Logger::getLogDate(timeout)) {
    date = utils::_tm_from_tp(psi_act::Clock::now());
  }

  std::stringstream ss;
  ss << std::put_time(&date, "%Y%m%d%H%M%S");
  sfs::path old_log_file_path = file_path;
  old_log_file_path += ".";
  old_log_file_path += ss.str();
  psi_act::log(
      file_stream_, backup_file_stream_, false, Logger::INFO, Logger::LOGGING,
      std::string("Rotating log file to ") + old_log_file_path.string());
  stream.close();

  if (utils::file::exists(old_log_file_path)) {
    utils::file::remove(old_log_file_path);
  }

#ifdef _WIN32
    const int status = _wrename(file_path.c_str(), old_log_file_path.c_str());
#else
    const int status = std::rename(file_path.c_str(), old_log_file_path.c_str());
#endif

  if (status != 0) {
    throw std::runtime_error("Failed copying log file " + file_path.string() +
                             " to " + old_log_file_path.string() +
                             ", check permissions? Return code " +
                             std::to_string(status));
    // TODO: This ^ should not throw. But.. how to recover?
  } else if (create_new) {
    // successfully copied old file, now open new one
    stream = std::ofstream(file_path.string(),
                           std::ofstream::out | std::ofstream::trunc);
    stream << std::left;
    date = Logger::getLogDate(timeout);
  }
}

Logger::~Logger() {
  // Logger instance is static, so there won't be concurring threads ... but
  // technically we need to lock
  std::lock_guard<std::mutex> lock(log_mutex_);

  // Rotate current log file to avoid overwrite and ensure log file is found by
  // collection service. But don't do it when rotation is disabled, as that is
  // confusing.
  if (rotation_timeout_ != NONE) {
    this->rotateLogFile(false, log_date_, rotation_timeout_, file_stream_,
                        file_path_);
  }

  file_stream_.close();

  if (backup_file_stream_ && backup_file_stream_.is_open()) {
    // Do NOT rotate the backup logfile, as that makes inspecting it with lnav
    // annoying.
    // if (backup_rotation_timeout_ != NONE) {
    //   this->rotateLogFile(false, backup_log_date_,
    //                       backup_rotation_timeout_,
    //                       backup_file_stream_,
    //                       backup_file_path_);
    // }
    backup_file_stream_.close();
  }
  stopped_ = true;
  worker_.join();
  queue_.close();
}

std::array<const char *, Logger::Level::_LAST_LEVEL> Logger::Level2String = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL", "ALWAYS"};

std::array<const char *, Logger::Identifier::_LAST_IDENTIFIER>
    Logger::Identifier2String = {
        "SENSOR",         "PROCESSING",      "BEHAVIOUR",
        "CONTROL_LOOP",   "WORLD_MODEL",     "CRANE",
        "COMMAND",        "DECISION_MAKING", "WORLD_OBSERVER",
        "CONFIGURATION",  "SAFETY",          "SETUP",
        "SHUTDOWN",       "LOGGING",         "POSTPROCESSING",
        "SENSOR_MANAGER", "CL_CLIENT",       "" /* NO_IDENTIFIER */};

std::map<Logger::Level, boost::circular_buffer<Logger::PriorityLog>>
    Logger::recent_high_prio_logs = {
        {WARN, boost::circular_buffer<Logger::PriorityLog>(10)},
        {ERROR, boost::circular_buffer<Logger::PriorityLog>(10)},
        {FATAL, boost::circular_buffer<Logger::PriorityLog>(10)}};

std::map<Logger::Level, long> Logger::high_prio_counts = {
    {WARN, 0}, {ERROR, 0}, {FATAL, 0}};

std::unique_ptr<Logger> Logger::instance{nullptr};

void Logger::init(const std::string &suffix) {
  Logger::instance = std::make_unique<Logger>(Level(0), true, true, suffix);
}

Logger &Logger::getLogger() {
  if (!Logger::instance) {
    throw exceptions::ConfigError(
        "Logger not yet initialized, only call LOG_* after Logger::init has "
        "been called.");
  }
  return *Logger::instance;
}

void Logger::setLogLevel(const std::string &level) {
  if (level == "TRACE") {
    log_level_ = TRACE;
  } else if (level == "INFO") {
    log_level_ = INFO;
  } else if (level == "WARN") {
    log_level_ = WARN;
  } else if (level == "ERROR") {
    log_level_ = ERROR;
  } else if (level == "FATAL") {
    log_level_ = FATAL;
  } else { // default
    log_level_ = DEBUG;
  }
  this->log(ALWAYS, Logger::LOGGING,
            std::string("Log level set to ") +
                Logger::Level2String[log_level_]);
}

Logger::RotationTimeout
Logger::parseRotationTimeout(const std::string &rotation) {
  if (rotation == "MINUTELY") {
    return MINUTELY;
  } else if (rotation == "HOURLY") {
    return HOURLY;
  } else if (rotation == "DAILY") {
    return DAILY;
  } else if (rotation == "MONTHLY") {
    return MONTHLY;
  } else if (rotation == "YEARLY") {
    return YEARLY;
  } else { // default
    return NONE;
  }
}

std::tm Logger::getLogDate(const RotationTimeout &rotation_timeout) {
  std::tm tm = utils::_tm_from_tp(psi_act::Clock::now());
  switch (rotation_timeout) {
  case MINUTELY:
    tm.tm_sec = 0;
    break;
  case HOURLY:
    tm.tm_sec = 0;
    tm.tm_min = 0;
    break;
  case DAILY:
    tm.tm_sec = 0;
    tm.tm_min = 0;
    tm.tm_hour = 0;
    break;
  case MONTHLY:
    tm.tm_sec = 0;
    tm.tm_min = 0;
    tm.tm_hour = 0;
    tm.tm_mday = 0;
    break;
  case YEARLY:
    tm.tm_sec = 0;
    tm.tm_min = 0;
    tm.tm_hour = 0;
    tm.tm_mday = 0;
    break;
  default:
    break;
  }
  return tm;
  /* std::stringstream ss; */
  /* ss << std::put_time(&tm, "%Y%m%d%H%M%S"); */
  /* return ss.str(); */
}

static void clearRolledLogfiles(const sfs::path &file_path,
                                size_t max_to_keep) {
  // 1. Collect rolled-away log files with a matching stem and an all-digit
  //    extension.
  std::vector<sfs::path> rolled_logfiles;
  const sfs::path parent =
      file_path.has_parent_path() ? file_path.parent_path() : sfs::path(".");
  const sfs::path stem = file_path.stem();
  for (sfs::directory_iterator end_dir_it, it(parent); it != end_dir_it; ++it) {
    const sfs::path &path = it->path();
    if (sfs::is_regular_file(path) && path.stem().stem() == stem) {
      const std::string ext = path.extension().string().substr(1);
      if (std::all_of(ext.begin(), ext.end(), ::isdigit)) {
        rolled_logfiles.emplace_back(path.filename());
      }
    }
  }
  if (!rolled_logfiles.empty()) {
    // 2. Sort the found files in ascending order.
    std::sort(rolled_logfiles.begin(), rolled_logfiles.end());
    // 3. Remove from the beginning of that list of files until only the desired
    //    amount remains.
    while (rolled_logfiles.size() > max_to_keep) {
      utils::file::remove(parent / rolled_logfiles[0]);
      rolled_logfiles.erase(rolled_logfiles.begin());
    }
  }
}

std::ostream &Logger::log(Level level, Identifier id,
                          const std::string &message) {
  // no sync necessary, do nothing and return the file stream for next use
  if (disabled || level < log_level_) {
    return file_stream_;
  }

  // Add log to queue, blocking if queue is full!
  queue_.push_back({CYCLE.counter, psi_act::Clock::now(), CYCLE.startedAt,
                    CYCLE.desiredEndAt, level, id, message});
  return file_stream_;
}

static std::ostream &log(std::ostream &file_stream,
                         std::ostream &file_backup_stream, bool flush_always,
                         const psi_act::Logger::LogItem &item) {
  const std::string date_time = utils::format_timepoint_iso8601(item.date_time);
  const std::string started_at =
      utils::format_timepoint_iso8601(item.started_at);
  const std::string desired_end_at =
      utils::format_timepoint_iso8601(item.desired_end_at);
  file_stream << std::setw(5) << item.counter << " | " << std::setw(15)
              << psi_act::Logger::Identifier2String[item.id] << " | "
              << date_time << " | " << started_at << " | " << desired_end_at
              << " | " << std::setw(7)
              << psi_act::Logger::Level2String[item.level] << " | "
              << item.message << "\n";

  if (file_backup_stream) {
    file_backup_stream << std::setw(5) << item.counter << " | " << std::setw(15)
                       << psi_act::Logger::Identifier2String[item.id] << " | "
                       << date_time << " | " << started_at << " | "
                       << desired_end_at << " | " << std::setw(7)
                       << psi_act::Logger::Level2String[item.level] << " | "
                       << item.message << "\n";
  }

  if (flush_always) {
    // Flush the output to the buffered stream
    file_stream.flush();
    if (file_backup_stream) {
      file_backup_stream.flush();
    }
  }

  return file_stream;
}

/**
 * @brief Helper function providing different arguments.
 */
std::ostream &log(std::ostream &file_stream, std::ostream &file_backup_stream,
                  bool flush_always, psi_act::Logger::Level level,
                  psi_act::Logger::Identifier id, const std::string &message) {
  psi_act::TimePoint time = psi_act::Clock::now();

  // Count the message and save it in a buffer if it's at WARN or above
  // The following seems to produce segfaults.
  // if (level >= Logger::Level::WARN) {
  //   Logger::high_prio_counts[level] += 1;
  //   Logger::PriorityLog entry{time, CYCLE.counter, message};
  //   Logger::recent_high_prio_logs[level].push_front(entry);
  // }

  const psi_act::TimePoint &date_time = time;
  const psi_act::TimePoint &started_at = CYCLE.startedAt;
  const psi_act::TimePoint &desired_end_at = CYCLE.desiredEndAt;
  return psi_act::log(file_stream, file_backup_stream, flush_always,
                      psi_act::Logger::LogItem{CYCLE.counter, date_time,
                                               started_at, desired_end_at,
                                               level, id, message});
}

void Logger::work_on_queue() {
  utils::set_thread_name("Logger");
  while (!stopped_) {
    LogItem log_item;
    auto status = queue_.try_pull_front(log_item);
    if (status != boost::queue_op_status::success) {
      std::this_thread::sleep_for(2ms);
      continue;
    }

    // Maybe rotate primary log file
    if (rotation_timeout_ != NONE &&
        !(log_date_ == getLogDate(rotation_timeout_))) {
      psi_act::log(
          file_stream_, backup_file_stream_, false, Logger::INFO,
          Logger::LOGGING,
          std::string("Rotating log file based on log rotation timeout: ") +
              file_path_.string());
      this->rotateLogFile(true, log_date_, rotation_timeout_, file_stream_,
                          file_path_);
    }
    // Maybe rotate backup log file
    if (backup_file_stream_ && backup_rotation_timeout_ != NONE &&
        !(backup_log_date_ == this->getLogDate(backup_rotation_timeout_))) {
      this->rotateLogFile(true, backup_log_date_, backup_rotation_timeout_,
                          backup_file_stream_, backup_file_path_);
      clearRolledLogfiles(backup_file_path_, backup_file_count_);
    }
    // Actually write the log line
    psi_act::log(file_stream_, backup_file_stream_, flush_always_, log_item);
  }
}

int Logger::get_queue_size() const { return queue_.size(); }

} // namespace psi_act
