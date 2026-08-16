//
//  Command.h
//  psi-act
//
//  Sascha Lange 2019.
//

#ifndef _PSIACT_TIME_H_
#define _PSIACT_TIME_H_

#include <chrono>

namespace psi_act {

using SystemTimePoint = std::chrono::system_clock::time_point;

using Clock = std::chrono::system_clock;
using TimePoint = std::chrono::system_clock::time_point;

class Duration {
    private:
        std::chrono::system_clock::duration duration_;

    public:
        // Constructors
        Duration() : duration_{0} {}

        Duration(const std::chrono::system_clock::duration d) : duration_{d} {}

        Duration(const std::chrono::minutes& minutes) : duration_{std::chrono::duration_cast<std::chrono::system_clock::duration>(minutes)} {}
        Duration(const std::chrono::seconds& seconds) : duration_{std::chrono::duration_cast<std::chrono::system_clock::duration>(seconds)} {}
        Duration(const std::chrono::milliseconds& milliseconds) : duration_{std::chrono::duration_cast<std::chrono::system_clock::duration>(milliseconds)} {}
        Duration(const std::chrono::microseconds& microseconds) : duration_{std::chrono::duration_cast<std::chrono::system_clock::duration>(microseconds)} {}
        #ifndef __APPLE__
        Duration(const std::chrono::nanoseconds& nanoseconds) : duration_{std::chrono::duration_cast<std::chrono::system_clock::duration>(nanoseconds)} {}
        #endif

        // // // Conversion from other compatible duration types
        // template <typename Rep, typename Period>
        // static Duration fromCompatibleDuration(const std::chrono::duration<Rep, Period>& d) {
        //     return Duration{std::chrono::duration_cast<std::chrono::system_clock::duration>(d)};
        // }

        // Conversion operator to std::chrono::system_clock::duration
        operator std::chrono::system_clock::duration() const {
            return duration_;
        }
    };
}

#endif /* _PSIACT_TIME_H_ */
