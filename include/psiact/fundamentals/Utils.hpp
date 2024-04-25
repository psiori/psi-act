#pragma once
#include <filesystem>

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

/**
 * @brief   Task our friends boyer and moore to find lines containing a search
 * string (no regex)
 *
 * @param stream    Opened binary stream
 * @param pattern   String to look for
 */
std::vector<std::string> matching_lines(std::ifstream &stream,
                                        const std::string &pattern);

/**
 * @brief Search directories created by a \ref autocrane::FolderIO to find the
 * newest file. This can also optionally ignore files older than a certain
 * cutoff, e.g. more than 5 minutes old. The path finding works by recursively
 * listing all files in the base directory , and then
 * finding the one with the largest seconds since epoch. If its newer than the
 * cutoff, the path to it is returned, otherwise an empty string is returned. If
 * a non-existent `base_directory` is passed, it will also return an empty
 * string. If the directories exist, but there are no files, it will also return
 * an empty etring.
 * @param base_directory Directory created by the \ref autocrane::FolderIO
 * @param cutoff How old something has to be in relation to now in order to be
 * considered expired and thus ignored. This can not be a negative number!
 * @param ext Extension to use for filtering files.
 * @return Path to latest file, or empty string if no valid latest file, no
 * directory, or not files found
 */
std::string path_latest_filename(const std::string &base_directory,
                                 const psi_act::Duration &cutoff,
                                 std::string ext = "");

} // namespace file
} // namespace utils
} // namespace psi_act
