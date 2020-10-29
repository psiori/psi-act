project(PSIACT-EXAMPLES CXX)

# if no option is given, standard is release
IF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Release)
ENDIF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

LIST(APPEND FSM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/fsm/main.cpp)

add_executable(fsm_demo ${FSM_SOURCES})
target_include_directories(fsm_demo PRIVATE ${PSIACT_SOURCE_DIR})
set(FSM_DEMO_LIBRARIES psiact)
target_link_libraries(fsm_demo ${FSM_DEMO_LIBRARIES})

LIST(APPEND FSD_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/state_deliberation/main.cpp)

add_executable(state_deliberation_demo ${FSD_SOURCES})
target_include_directories(state_deliberation_demo PRIVATE ${PSIACT_SOURCE_DIR})
set(FSD_DEMO_LIBRARIES psiact)
target_link_libraries(state_deliberation_demo ${FSD_DEMO_LIBRARIES})

INSTALL(TARGETS fsm_demo
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

INSTALL(TARGETS state_deliberation_demo
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

set_source_files_properties(${FSM_SOURCES} PROPERTIES COMPILE_OPTIONS -Wno-error)
set_source_files_properties(${FSM_SOURCES} PROPERTIES COMPILE_OPTIONS -Wno-error=old-style-cast)

set_source_files_properties(${FSD_SOURCES} PROPERTIES COMPILE_OPTIONS -Wno-error)
set_source_files_properties(${FSD_SOURCES} PROPERTIES COMPILE_OPTIONS -Wno-error=old-style-cast)


# add_lint_target(psiact-library-lint "${FILES}")
# set_property(GLOBAL APPEND PROPERTY LINT_TARGETS psiact-library-lint)
