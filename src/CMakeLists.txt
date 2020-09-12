project(PSIACT CXX)

# if no option is given, standard is release
IF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Release)
ENDIF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS_RELEASE " -Wall -O2 -mavx")
set(CMAKE_CXX_FLAGS_DEBUG " -Wall -pg")

LIST(APPEND PSIACT_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/Command.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/CycleManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/arbitration/Option.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/arbitration/PriorityArbitration.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/arbitration/SequentialPlan.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/arbitration/Arbitrator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/deliberation/Deliberator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/deliberation/FiniteStateMachine.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/TimePoint.cpp
)

LIST(APPEND PSIACT_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/Command.h
  ${CMAKE_CURRENT_SOURCE_DIR}/CycleManager.h
  ${CMAKE_CURRENT_SOURCE_DIR}/arbitration/Option.h
  ${CMAKE_CURRENT_SOURCE_DIR}/arbitration/PriorityArbitration.h
  ${CMAKE_CURRENT_SOURCE_DIR}/arbitration/SequentialPlan.h
  ${CMAKE_CURRENT_SOURCE_DIR}/arbitration/Arbitrator.h
  ${CMAKE_CURRENT_SOURCE_DIR}/deliberation/Deliberator.h
  ${CMAKE_CURRENT_SOURCE_DIR}/deliberation/FiniteStateMachine.h
  ${CMAKE_CURRENT_SOURCE_DIR}/TimePoint.h
)

LIST(APPEND FILES
  ${PSIACT_SRCS}
  ${PSIACT_HEADERS})


# define subgroups for XCode and other IDEs
source_group( src FILES ${PSIACT_HEADERS} ${PSIACT_SRCS} )

set(PSIACT_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

ADD_LIBRARY(psiact STATIC ${PSIACT_SRCS})

target_include_directories(psiact PRIVATE ${PSIACT_INCLUDE_DIRS})

set_target_properties(psiact PROPERTIES COMPILE_FLAGS "-std=c++11")



INSTALL(FILES ${PSIACT_HEADERS} DESTINATION include)

INSTALL(TARGETS psiact
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# add_lint_target(psiact-library-lint "${FILES}")
# set_property(GLOBAL APPEND PROPERTY LINT_TARGETS psiact-library-lint)
