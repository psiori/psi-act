project(PSIACT LANGUAGES CXX)

# if no option is given, standard is release
IF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Release)
ENDIF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -O0 -g -pg")

LIST(APPEND PSIACT_SRCS
  ${CMAKE_CURRENT_LIST_DIR}/Command.cpp
  ${CMAKE_CURRENT_LIST_DIR}/CycleManager.cpp
  ${CMAKE_CURRENT_LIST_DIR}/arbitration/Option.cpp
  ${CMAKE_CURRENT_LIST_DIR}/arbitration/PriorityArbitration.cpp
  ${CMAKE_CURRENT_LIST_DIR}/arbitration/SequentialPlan.cpp
  ${CMAKE_CURRENT_LIST_DIR}/arbitration/Arbitrator.cpp
  ${CMAKE_CURRENT_LIST_DIR}/deliberation/Deliberator.cpp
  ${CMAKE_CURRENT_LIST_DIR}/deliberation/FiniteStateMachine.cpp
  ${CMAKE_CURRENT_LIST_DIR}/deliberation/FiniteStateDeliberation.cpp
  ${CMAKE_CURRENT_LIST_DIR}/TimePoint.cpp
)

LIST(APPEND PSIACT_HEADERS
  ${CMAKE_CURRENT_LIST_DIR}/Command.h
  ${CMAKE_CURRENT_LIST_DIR}/CycleManager.h
  ${CMAKE_CURRENT_LIST_DIR}/arbitration/Option.h
  ${CMAKE_CURRENT_LIST_DIR}/arbitration/PriorityArbitration.h
  ${CMAKE_CURRENT_LIST_DIR}/arbitration/SequentialPlan.h
  ${CMAKE_CURRENT_LIST_DIR}/arbitration/Arbitrator.h
  ${CMAKE_CURRENT_LIST_DIR}/deliberation/Deliberator.h
  ${CMAKE_CURRENT_LIST_DIR}/deliberation/FiniteStateMachine.h
  ${CMAKE_CURRENT_LIST_DIR}/deliberation/FiniteStateDeliberation.h
  ${CMAKE_CURRENT_LIST_DIR}/TimePoint.h
)

LIST(APPEND FILES
  ${PSIACT_SRCS}
  ${PSIACT_HEADERS})


# define subgroups for XCode and other IDEs
source_group( src FILES ${PSIACT_HEADERS} ${PSIACT_SRCS} )

set(PSIACT_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR};${CMAKE_CURRENT_LIST_DIR}/deliberation;${CMAKE_CURRENT_LIST_DIR}/arbitration)

FIND_PACKAGE(Boost REQUIRED)
ADD_LIBRARY(psiact STATIC ${PSIACT_SRCS})
target_compile_options(psiact PRIVATE -Wno-error -Wno-error=old-style-cast)

message(STATUS "PSIACT_INCLUDE_DIRS: ${PSIACT_INCLUDE_DIRS}")
# Include PUBLICly so including projects get these as well
target_include_directories(psiact PUBLIC ${PSIACT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
target_link_libraries(psiact PUBLIC ${Boost_LIBRARIES})

INSTALL(FILES ${PSIACT_HEADERS} DESTINATION include)

INSTALL(TARGETS psiact
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
