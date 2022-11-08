//
//  Deliberator.cpp
//  psi-act
//
//  Sascha Lange, 2020
//

#include "Deliberator.h"


psi_act::Deliberator::Deliberator(const std::string& name) : name(name) {
}


// ////////////////////////////////////////////////////////////////////////////
//
//    D E S I R E   I N F O R M A T I O N
//
// ////////////////////////////////////////////////////////////////////////////

psi_act::DesireInformation::DesireInformation() : desire("_UNDEFINED") {
}


psi_act::DesireInformation::DesireInformation(const std::string& desire,
                                              const TimePoint& presentTime,
                                              long presentCycle) :
    desire(desire), startedAt(presentTime), startedAtCycle(presentCycle) {
}
