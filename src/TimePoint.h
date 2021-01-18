//
//  Command.h
//  psi-act
//
//  Sascha Lange 2019.
//

/**
 * \file TimePoint.h
 * \brief Wrapper classes for several std::chrono::system_clock classes.
 */

#ifndef _PSIACT_TIME_H_
#define _PSIACT_TIME_H_

#include <chrono>

namespace psi_act {

using SystemTimePoint = std::chrono::system_clock::time_point;

using Duration  = std::chrono::system_clock::duration;
using Clock     = std::chrono::system_clock;
using TimePoint = std::chrono::system_clock::time_point;

}  // namespace psi_act

#endif /* _PSIACT_TIME_H_ */
