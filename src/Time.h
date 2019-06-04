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

 using Duration = std::chrono::system_clock::duration;
 using SystemTimePoint = std::chrono::system_clock::time_point;

 /**
  * @class TimePoint
  * @brief Subtype of system clock time point which we can add functionality to
  */
 class TimePoint : public std::chrono::system_clock::time_point {
  public:
   TimePoint() : SystemTimePoint(Duration(0)) {};
   TimePoint(Duration d) : SystemTimePoint(d) {};
   static TimePoint now();
  };
}

#endif /* Command_h */
