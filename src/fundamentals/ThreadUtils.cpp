// Based off of https://stackoverflow.com/a/23899379

#include "psiact/fundamentals/ThreadUtils.hpp"

namespace psi_act {
namespace utils {

#ifdef _WIN32
// https://learn.microsoft.com/en-gb/previous-versions/visualstudio/visual-studio-2015/debugger/how-to-set-a-thread-name-in-native-code?view=vs-2015&redirectedfrom=MSDN

#include <windows.h>
const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO {
  DWORD dwType;     // Must be 0x1000.
  LPCSTR szName;    // Pointer to name (in user addr space).
  DWORD dwThreadID; // Thread ID (-1=caller thread).
  DWORD dwFlags;    // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)
void set_thread_name(const std::string &thread_name) {
  THREADNAME_INFO info;
  info.dwType = 0x1000;
  info.szName = thread_name.c_str();
  info.dwThreadID = GetCurrentThreadId();
  info.dwFlags = 0;
#pragma warning(push)
#pragma warning(disable : 6320 6322)
  __try {
    RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR),
                   (ULONG_PTR *)&info);
  } __except (EXCEPTION_EXECUTE_HANDLER) {
  }
#pragma warning(pop)
}

#elif defined(__APPLE__)
// https://opensource.apple.com/source/libpthread/libpthread-218.1.3/src/pthread.c.auto.html

#include <pthread.h>
void set_thread_name(const std::string &thread_name) {
  pthread_setname_np(thread_name.c_str());
}

#else
// https://man7.org/linux/man-pages/man2/prctl.2.html
// https://man7.org/linux/man-pages/man3/pthread_setname_np.3.html

#include <sys/prctl.h>
void set_thread_name(const std::string &thread_name) {
  prctl(PR_SET_NAME, thread_name.c_str(), 0, 0, 0);
  // libzmq is using the following, but somehow that does not work for us?
  // https://github.com/zeromq/libzmq/blob/532b6127/src/thread.cpp#L377-L410
  // pthread_setname_np(pthread_self(), thread_name.c_str());
}

#endif

#ifdef _WIN32
// consider using SetPriorityClass() on windows
bool set_process_niceness(int value) { return false; }

#else

#include <sys/resource.h>
bool set_process_niceness(int value) {
  return setpriority(PRIO_PROCESS, 0, value) == 0;
}

#endif

} // namespace utils
} // namespace psi_act
