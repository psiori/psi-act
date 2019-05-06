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
  typedef std::chrono::high_resolution_clock Clock;
  typedef std::chrono::high_resolution_clock::time_point TimePoint;
}

#endif /* Command_h */

