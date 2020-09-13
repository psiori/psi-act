project(PSIACT-EXAMPLES CXX)

# if no option is given, standard is release
IF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Release)
ENDIF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS_RELEASE " -Wall -O2 -mavx")
set(CMAKE_CXX_FLAGS_DEBUG " -Wall -pg")

LIST(APPEND FSM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/fsm/main.cpp)

add_executable(fsm_demo ${FSM_SOURCES})

target_include_directories(fsm_demo PRIVATE ${PSIACT_SOURCE_DIR})

set(FSM_DEMO_LIBRARIES psiact)

target_link_libraries(fsm_demo ${FSM_DEMO_LIBRARIES})


INSTALL(TARGETS fsm_demo
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# add_lint_target(psiact-library-lint "${FILES}")
# set_property(GLOBAL APPEND PROPERTY LINT_TARGETS psiact-library-lint)
