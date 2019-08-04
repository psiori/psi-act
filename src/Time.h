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

using Duration = std::chrono::system_clock::duration;
using Clock = std::chrono::system_clock;
using TimePoint = std::chrono::system_clock::time_point;

/// TODO(sascha@psiori.com): if we really want to subclass (STL not really made for subclassing), than let's do it correctly without breaking stuff.
/*
class TimePoint : public std::chrono::system_clock::time_point {
public:
  TimePoint() : SystemTimePoint(Duration(0)) {};
  TimePoint(Duration d) : SystemTimePoint(d) {}; 
  static TimePoint now();
};*/

}

#endif /* _PSIACT_TIME_H_ */
