#include <psiact/fundamentals/Utils.hpp>
#include <unistd.h>
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

} // namespace file

} // namespace utils
} // namespace psi_act
