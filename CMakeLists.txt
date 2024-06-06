cmake_minimum_required(VERSION 3.15.0)
project(
  psiact
  VERSION 0.0.1
  LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "-- psiact ------------------------------------------------")

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)

option(BUILD_BEHAVIORS "Build the cognitive architecture stack" ON)
option(BUILD_UTILS "Build non-psi-act-related utilities" ON)

if(BUILD_BEHAVIORS)
  set(BUILD_UTILS ON)
endif()

if(BUILD_UTILS)
  add_library(psiact-utils)
  target_sources(
    psiact-utils
    PRIVATE ${SRC_DIR}/utils/Logger.cpp
    ${SRC_DIR}/utils/YAMLReader.cpp
    ${SRC_DIR}/fundamentals/ConfigNode.cpp
    ${SRC_DIR}/fundamentals/ThreadUtils.cpp
    ${SRC_DIR}/fundamentals/TimeUtils.cpp
    ${SRC_DIR}/fundamentals/Utils.cpp
    ${SRC_DIR}/CycleManager.cpp)
  target_sources(
    psiact-utils
    PUBLIC FILE_SET
    psiact_utils_headers
    TYPE
    HEADERS
    BASE_DIRS
    ${INCLUDE_DIR}
    FILES
    ${INCLUDE_DIR}/psiact/utils/Logger.hpp
    ${INCLUDE_DIR}/psiact/utils/YAMLReader.hpp
    ${INCLUDE_DIR}/psiact/utils/log_utils.hpp
    ${INCLUDE_DIR}/psiact/fundamentals/ConfigNode.hpp
    ${INCLUDE_DIR}/psiact/CycleManager.h
    ${INCLUDE_DIR}/psiact/TimePoint.h
    ${INCLUDE_DIR}/psiact/fundamentals/Exceptions.hpp
    ${INCLUDE_DIR}/psiact/fundamentals/StreamUtils.hpp
    ${INCLUDE_DIR}/psiact/fundamentals/ThreadUtils.hpp
    ${INCLUDE_DIR}/psiact/fundamentals/TimeUtils.hpp
    ${INCLUDE_DIR}/psiact/fundamentals/Utils.hpp)
  target_include_directories(
    psiact-utils SYSTEM PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
  set(CMAKE_FIND_DEBUG_MODE TRUE)
  find_package(yaml-cpp REQUIRED)
  set(CMAKE_FIND_DEBUG_MODE OFF)

  if(NOT YAML_CPP_INCLUDE_DIR)
    get_property(
      YAML_INCLUDE_PROP
      TARGET yaml-cpp
      PROPERTY INTERFACE_INCLUDE_DIRECTORIES
    )
    set(YAML_CPP_INCLUDE_DIR ${YAML_INCLUDE_PROP})
    get_property(
      YAML_LIB_DIR_PROP
      TARGET yaml-cpp
      PROPERTY INTERFACE_LINK_DIRECTORIES
    )
    set(YAML_CPP_LIBRARY_DIR ${YAML_LIB_DIR_PROP})
  endif()

  # only include yaml dir in build tree. I am not sure what should be included in
  # install tree, since when package is relocated, the yaml headers might be
  # anywhere. I think one would just juse CMAKE_INSTALL_INCLUDEDIR (which we already
  # have), as that would be appended to every search prefix. To find yml, the prefix
  # would have been set by the user and thus would be prepended to
  # CMAKE_INSTALL_INCLUDEDIR (='include/' usually) and there yml headers will be found.
  target_include_directories(psiact-utils SYSTEM PUBLIC
    $<BUILD_INTERFACE:${YAML_CPP_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
  message(STATUS "yaml-cpp headers at ${YAML_CPP_INCLUDE_DIR}")
  target_link_directories(psiact-utils PUBLIC ${YAML_CPP_LIBRARY_DIR})
  message(STATUS "yaml-cpp libs at ${YAML_CPP_LIBRARY_DIR}")

  find_package(Boost REQUIRED)
  target_link_libraries(psiact-utils PUBLIC yaml-cpp ${Boost_LIBRARIES})
  target_include_directories(psiact-utils PUBLIC ${Boost_INCLUDE_DIRS})
  install(
    TARGETS psiact-utils
    EXPORT ${PROJECT_NAME}_Targets
    FILE_SET psiact_utils_headers
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()

if(BUILD_BEHAVIORS)
  add_library(psiact-behavior)
  target_sources(
    psiact-behavior
    PRIVATE ${SRC_DIR}/Command.cpp
    ${SRC_DIR}/arbitration/Option.cpp
    ${SRC_DIR}/arbitration/PriorityArbitration.cpp
    ${SRC_DIR}/arbitration/SequentialPlan.cpp
    ${SRC_DIR}/arbitration/Arbitrator.cpp
    ${SRC_DIR}/arbitration/FiniteStateArbitration.cpp
    ${SRC_DIR}/deliberation/Deliberator.cpp
    ${SRC_DIR}/deliberation/FiniteStateMachine.cpp
    ${SRC_DIR}/deliberation/FiniteStateDeliberation.cpp)
  target_sources(
    psiact-behavior
    PUBLIC FILE_SET
    psiact_behavior_headers
    TYPE
    HEADERS
    BASE_DIRS
    ${INCLUDE_DIR}
    FILES
    ${INCLUDE_DIR}/psiact/Command.h
    ${INCLUDE_DIR}/psiact/deliberation
    ${INCLUDE_DIR}/psiact/arbitration)
  find_package(Boost REQUIRED)
  target_link_libraries(psiact-behavior PUBLIC ${Boost_LIBRARIES})
  target_include_directories(psiact-behavior PUBLIC ${Boost_INCLUDE_DIRS})
  target_link_libraries(psiact-behavior PUBLIC psiact-utils)

  target_include_directories(
    psiact-behavior SYSTEM
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
  install(
    TARGETS psiact-behavior
    EXPORT ${PROJECT_NAME}_Targets
    FILE_SET psiact_behavior_headers
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

if(BUILD_BEHAVIORS)
  target_link_libraries(${PROJECT_NAME} INTERFACE psiact-behavior)
endif()

if(BUILD_UTILS)
  target_link_libraries(${PROJECT_NAME} INTERFACE psiact-utils)
endif()

install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}_Targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion)

configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
  ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION ${INSTALL_CONFIGDIR})

install(
  EXPORT ${PROJECT_NAME}_Targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${INSTALL_CONFIGDIR})

install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${INSTALL_CONFIGDIR})

# ##############################################################################
#
# FIND OPTIONAL PACKAGES
#
# ##############################################################################
message(STATUS)
message(STATUS "Searching for additional packages ------------------------")

option(TESTING "Set to OFF to prevent compilation of unit tests." OFF)

message(STATUS)
message(STATUS "-- PSI-ACT Build Options ---------------------------------")
message(STATUS "CMAKE_INSTALL_PREFIX   = ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_PREFIX_PATH      = ${CMAKE_PREFIX_PATH}")
message(STATUS "TESTING                = ${TESTING}")
message(STATUS "CMAKE_BUILD_TYPE       = ${CMAKE_BUILD_TYPE}")
message(STATUS "Change a value with: cmake -D<VAR>=<VALUE>")
message(STATUS "----------------------------------------------------------")
message(STATUS)

# doc target
option(BUILD_DOC "Build the doxygen doc" OFF)

if(BUILD_DOC)
  find_package(Doxygen REQUIRED)
  find_package_handle_standard_args(Doxygen DEFAULT_MSG DOXYGEN_EXECUTABLE)

  add_custom_target(
    doc-psiact
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# ##############################################################################
#
# TESTING
#
# ##############################################################################
if(TESTING)
  if(NOT TARGET gtest)
    # We need thread support
    find_package(Threads REQUIRED)

    # Enable ExternalProject CMake module
    include(ExternalProject)

    # Download and install GoogleTest
    ExternalProject_Add(
      psi-act-gtest
      URL https://github.com/google/googletest/archive/main.zip
      PREFIX ${PROJECT_BINARY_DIR}/psi-act-gtest

      # Disable install step
      INSTALL_COMMAND "")

    # Get GTest source and binary directories from CMake project
    ExternalProject_Get_Property(psi-act-gtest SOURCE_DIR BINARY_DIR)

    # Create a libgtest target to be used as a dependency by test programs
    add_library(gtest IMPORTED STATIC GLOBAL)
    add_dependencies(gtest psi-act-gtest)

    # Set libgtest properties
    set_target_properties(
      gtest
      PROPERTIES "IMPORTED_LOCATION" "${BINARY_DIR}/lib/libgtest.a"
      "IMPORTED_LINK_INTERFACE_LIBRARIES"
      "${CMAKE_THREAD_LIBS_INIT}")

    # Create a libgmock target to be used as a dependency by test programs
    add_library(gmock IMPORTED STATIC GLOBAL)
    add_dependencies(gmock psi-act-gtest)

    # Set libgmock properties
    set_target_properties(
      gmock
      PROPERTIES "IMPORTED_LOCATION" "${BINARY_DIR}/lib/libgmock.a"
      "IMPORTED_LINK_INTERFACE_LIBRARIES"
      "${CMAKE_THREAD_LIBS_INIT}")

    set(_GTEST_INCLUDE_DIRS ${SOURCE_DIR}/googletest/include)
    set(_GMOCK_INCLUDE_DIRS ${SOURCE_DIR}/googlemock/include)
    set(GTEST_INCLUDE_DIRS ${_GTEST_INCLUDE_DIRS} ${_GMOCK_INCLUDE_DIRS})

    # directories must exist to use INTERFACE inlcude dirs, but they are only
    # created at build time. so we manually create at configure time to make
    # cmake happy.
    file(MAKE_DIRECTORY ${_GTEST_INCLUDE_DIRS})
    file(MAKE_DIRECTORY ${_GMOCK_INCLUDE_DIRS})

    # I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
    target_include_directories(gtest INTERFACE ${GTEST_INCLUDE_DIRS})
  endif()

  enable_testing()
  add_subdirectory(test)
endif() # testing

# ##############################################################################
#
# LINT
#
# ##############################################################################
get_property(LINT_TARGETS GLOBAL PROPERTY LINT_TARGETS)

if(NOT TARGET psi-act-lint)
  add_custom_target(
    psi-act-lint ALL
    DEPENDS ${LINT_TARGETS}
    VERBATIM)
endif()

message(STATUS "-- psiact ------------------------------------------------")
message(STATUS "----------------------------------------------------------")
message(STATUS)
