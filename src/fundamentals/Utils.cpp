#pragma once
#include <psiact/fundamentals/Utils.hpp>

namespace sfs = std::filesystem;
namespace psi_act {
namespace utils {
namespace file {

bool exists(const sfs::path &path) {
  // The value of amode is either the bitwise-inclusive OR of the access
  // permissions to be checked (R_OK, W_OK, X_OK) or the existence test (F_OK).
  return 0 == access(path.string().c_str(), F_OK);
}

bool create_directories(const sfs::path &path) {
  sfs::path tmp = "";
  int status = -1;
  // create directory hierarchy iteratively
  for (const sfs::path &part : path) {
    tmp /= part;
#if defined(_WIN32) && _WIN32
    status = create_directory(tmp);
#else
    // ACCESSPERMS: UNIX permission settings equivalent of 0777 = rwxrwxrwx
    status = mkdir(tmp.c_str(), ACCESSPERMS);
#endif
  }
  // When status is 0, mkdir was successful.
  // Note that the status may be non-zero intermediately in the loop above, as
  // individual pieces of the directory hierarchy might already exist. The
  // method still returns true as long as the final directory could be created.
  return 0 == status;
}

bool remove(const sfs::path &path) {
  try {
    // ::remove returns 0 on success, anything else on failure.
    return 0 == ::remove(path.string().c_str()); // explicitly use global scope
  } catch (const std::exception &) {
    return false;
  }
}

bool copy(const sfs::path &src, const sfs::path &dst) {
  std::ifstream input_file{src.string()};
  std::ofstream output_file{dst.string()};
  try {
    output_file << input_file.rdbuf();
  } catch (const std::exception &) {
    return false;
  }
  return true;
}

std::vector<char> read_file(std::ifstream &stream) {
  if (!stream.is_open()) {
    throw std::invalid_argument("Stream not opened.");
  }
  const std::streamsize size = stream.tellg();
  stream.seekg(0, std::ios::beg);
  std::vector<char> buf(size);
  stream.read(buf.data(), size);
  return buf;
}

std::vector<char> read_file(const std::string &file) {
  std::ifstream stream(file, std::ios::binary | std::ios::ate);
  return read_file(stream);
}

std::vector<std::string> matching_lines(std::ifstream &stream,
                                        const std::string &pattern) {
  using namespace std;
  vector<char> file_mem = read_file(stream);

  using iter_t = decltype(file_mem)::const_iterator;

  const string &pattern_line_s = pattern;
  const vector<char> pattern_line(pattern_line_s.begin(), pattern_line_s.end());

  boost::algorithm::boyer_moore<iter_t> line_finder(pattern_line.begin(),
                                                    pattern_line.end());
  vector<string> ret;

  iter_t current_pos = file_mem.begin();
  const iter_t end = file_mem.end();
  while (true) {
    iter_t match_begin, match_end;
    tie(match_begin, match_end) = line_finder(current_pos, end);
    if (match_begin == file_mem.end()) {
      break;
    } else {
      // line matches, extract the json part
      const iter_t line_end = std::find(match_begin, end, '\n');

      if (line_end == file_mem.end()) {
        throw std::runtime_error("Could not find line ending: " +
                                 string(match_begin, end));
      }

      iter_t last_line_end;
      last_line_end = match_begin - 1;
      while (true) {
        if (last_line_end == file_mem.begin()) {
          break;
        } else if (*last_line_end == '\n') {
          ++last_line_end;
          break;
        } else {
          --last_line_end;
        }
      }

      ret.emplace_back(last_line_end, line_end);
      last_line_end = line_end + 1;
      current_pos = line_end + 1;
    }
  }

  return ret;
}

std::string path_latest_filename(const std::string &base_directory,
                                 const psi_act::Duration &cutoff,
                                 std::string ext) {
  if (!exists(base_directory)) {
    return std::string(); // Empty string means "not found".
  }

  // Make sure extension is provided with a dot.
  if (!ext.empty() && !ext.starts_with('.')) {
    ext = "." + ext;
  }

  // We want to compare seconds since the epoch for the cutoff date and the
  // latest date in the folder. Get the expiry epoch seconds by subtracting the
  // cutoff from the current time.
  const psi_act::TimePoint now_t = std::chrono::system_clock::now();
  const auto expiry_date_epoch_s = utils::seconds_since_epoch(now_t - cutoff);

  // Loop through all contents of dir_path recursively and collect filenames
  std::vector<sfs::path> file_paths;
  sfs::recursive_directory_iterator end_itr; // default ctor = end
  for (sfs::recursive_directory_iterator itr(base_directory); itr != end_itr;
       ++itr) {
    LOG_TRACE(Logger::WORLD_MODEL,
              "path_latest_filename() considering " << itr->path());
    // Save the path if the path is a file
    if (is_regular_file(itr->path())) {
      file_paths.push_back(itr->path());
    }
  }
  if (file_paths.empty()) {
    // No files found
    return std::string();
  }

  // Extract the dates from each filename, and convert them to epoch seconds. We
  // can then check for the latest file by just finding the max date number.
  // Example filename: 2021-07-14T10_08_01_937Z.pcd
  std::vector<long> date_numbers;
  std::vector<sfs::path>::iterator it = file_paths.begin();
  while (it != file_paths.end()) {
    // Ignore files which have an undesired extension.
    if (!ext.empty() && (*it).extension() != ext) {
      it = file_paths.erase(it);
      continue;
    }

    // Extract the filename by dropping the path part
    const std::string file_string = (*it).stem().string();

    // Transform string date into a date object; utc! Ignore files which do not
    // have a proper ISO8601 date as filename.
    psi_act::TimePoint file_date;
    try {
      constexpr bool sanitized = true;
      file_date = utils::parse_timepoint_iso8601(file_string, sanitized);
    } catch (const std::invalid_argument &) {
      it = file_paths.erase(it);
      continue; // Ignore invalid file names.
    }

    // Convert to seconds since epoch
    date_numbers.push_back(utils::seconds_since_epoch(file_date));
    ++it;
  }

  // Empty set, nothing to return.
  if (date_numbers.empty()) {
    return std::string(); // Empty string means "nothing found".
  }

  // Make the comparison
  const int latest_file_index = utils::argmax(date_numbers);
  const long latest_date_epoch_s = date_numbers[latest_file_index];
  if (latest_date_epoch_s >= expiry_date_epoch_s) {
    return file_paths[latest_file_index].string();
  }

  return std::string(); // Empty string means "too old".
}

} // namespace file

} // namespace utils
} // namespace psi_act
