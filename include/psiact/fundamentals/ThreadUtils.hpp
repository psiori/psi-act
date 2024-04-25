// Based off of https://stackoverflow.com/a/23899379

#pragma once

#include <string>

namespace psi_act {
namespace utils {

/**
 * @brief Set the name of the current thread.
 *
 * Name appears for example during debugging using lldb or gcc. Tries to work on
 * windows, ubuntu and macos, never tested on windows tho.
 *
 * @param thread_name String to use as thread name
 */
void set_thread_name(const std::string &thread_name);

/**
 * @brief Set the priority of the current process in terms of 'niceness'.
 *
 * The lower the number, the more important this program is to the scheduler.
 */
bool set_process_niceness(int value);

} // namespace utils
} // namespace psi_act
