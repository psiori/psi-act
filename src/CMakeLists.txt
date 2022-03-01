project(psiact LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)

# if no option is given, standard is release
IF(NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Release)
ENDIF()

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

# disable flags on msvc
if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(psiact PRIVATE -Wno-error -Wno-error=old-style-cast)
endif()

message(STATUS "PSIACT_INCLUDE_DIRS: ${PSIACT_INCLUDE_DIRS}")
# Include PUBLICly so including projects get these as well
target_include_directories(psiact PUBLIC ${PSIACT_INCLUDE_DIRS})
target_include_directories(psiact SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(psiact PUBLIC ${Boost_LIBRARIES})

# Install headers into include/psiact/*, keeping the src directory structure.
foreach(file ${PSIACT_HEADERS})
  get_filename_component(dir ${file} DIRECTORY BASE_DIR ${CMAKE_CURRENT_LIST_DIR})
  string(REGEX REPLACE "^${CMAKE_CURRENT_LIST_DIR}" "" dir "${dir}")
  string(REGEX REPLACE "^/" "" dir "${dir}")
  install(FILES ${file} DESTINATION include/${CMAKE_PROJECT_NAME}/${dir})
endforeach()

INSTALL(TARGETS psiact
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
