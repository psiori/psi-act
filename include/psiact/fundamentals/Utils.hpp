#pragma once
#include "psiact/TimePoint.h"
#include <algorithm> // std::sort, std::stable_sort
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric> // std::iota
#include <psiact/TimePoint.h>
#include <set>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unordered_map>
#include <vector>

namespace sfs = std::filesystem;
namespace psi_act {
namespace utils {
namespace file {
/**
 * @brief Check whether a file or directory at the given \p path exists.
 * Workaround for sfs::exists not being implemented for jetson/arm64.
 * @param path File to check.
 * @returns True if \p path exists, false otherwise.
 */
bool exists(const sfs::path &path);

/**
 * @brief Create directory hierarchy with given \p path.
 * Workaround for sfs::exists not being implemented for jetson/arm64.
 * @param path Deep directory path to create.
 * @returns True if directories were created successfully, false otherwise.
 *    False if directory already exists.
 */
bool create_directories(const sfs::path &path);

/**
 * @brief Remove file or directory at given \p path. Note that for directories
 * this currently cannot delete non-empty directories!
 * Workaround for sfs::remove not being implemented for jetson/arm64.
 * @param path Path that should be removed.
 * @return True if \p path was removed successfully.
 */
bool remove(const sfs::path &path);

/**
 * @brief Copy a file from \p src to \p dst. Does not work for directories.
 * Workaround for sfs::copy not being implemented for jetson/arm64.
 * @param src Source path
 * @param dst Destination path
 * @returns True for successfully copying file \p src to \p dst
 */
bool copy(const sfs::path &src, const sfs::path &dst);

/**
 * @brief Read entire file into memory and return as char buffer.
 * @param file Path to the file.
 * @note Does not do any error checking, will crash on invalid file.
 */
std::vector<char> read_file(const std::string &file);

/**
 * @brief Read entire file into memory and return as char buffer.
 * @param file File stream.
 * @note Does not do any error checking, will crash on invalid file.
 */
std::vector<char> read_file(std::ifstream &stream);

} // namespace file
} // namespace utils
} // namespace psi_act
