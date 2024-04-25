cmake_minimum_required(VERSION 3.15.0)
project(psiact VERSION 0.0.1 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# if no option is given, standard is release
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF()

message(STATUS)
message(STATUS "----------------------------------------------------------")
message(STATUS "-- psiact ------------------------------------------------")

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)

add_library(${PROJECT_NAME})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

LIST(APPEND PSIACT_SRCS
  ${SRC_DIR}/Command.cpp
  ${SRC_DIR}/CycleManager.cpp
  ${SRC_DIR}/arbitration/Option.cpp
  ${SRC_DIR}/arbitration/PriorityArbitration.cpp
  ${SRC_DIR}/arbitration/SequentialPlan.cpp
  ${SRC_DIR}/arbitration/Arbitrator.cpp
  ${SRC_DIR}/arbitration/FiniteStateArbitration.cpp
  ${SRC_DIR}/deliberation/Deliberator.cpp
  ${SRC_DIR}/deliberation/FiniteStateMachine.cpp
  ${SRC_DIR}/deliberation/FiniteStateDeliberation.cpp
  ${SRC_DIR}/utils/Logger.cpp
  ${SRC_DIR}/utils/YAMLReader.cpp
  ${SRC_DIR}/fundamentals/ConfigNode.cpp
  ${SRC_DIR}/fundamentals/ThreadUtils.cpp
  ${SRC_DIR}/fundamentals/TimeUtils.cpp
)

target_sources(${PROJECT_NAME} PRIVATE ${PSIACT_SRCS})
target_compile_definitions(${PROJECT_NAME} PRIVATE VERSION="${CMAKE_PROJECT_VERSION}")

# Inclusion of headers both for building the target and for linking against it. Use
# INTERFACE instead of PUBLIC if the include dirs are required for dependent projects,
# not for this project itself.
target_include_directories(
  ${PROJECT_NAME}
  SYSTEM
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:include>)

find_package(Boost)
message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
message(STATUS "Boost library dirs: ${Boost_LIBRARY_DIRS}")

# disable flags on msvc
if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  target_compile_options(psiact PRIVATE -Wno-error -Wno-error=old-style-cast)
endif()

message(STATUS "PSIACT BOOST: ${Boost_LIBRARIES} ${Boost_INCLUDE_DIRS}")

find_package(yaml-cpp CONFIG REQUIRED)
message(STATUS "PSIACT YAML-CPP: ${YAML_CPP_INCLUDE_DIR} ${YAML_CPP_LIBRARIES}")

if(NOT YAML_CPP_INCLUDE_DIR)
  get_property(
    YAML_INCLUDE_PROP
    TARGET yaml-cpp
    PROPERTY INTERFACE_INCLUDE_DIRECTORIES
  )
  message(STATUS "Yaml-cpp include prop = ${YAML_INCLUDE_PROP}")
  set(YAML_CPP_INCLUDE_DIR ${YAML_INCLUDE_PROP})
endif()

if(NOT YAML_CPP_LIBRARIES)
  message(STATUS "Yaml-cpp hardcoding empty libraries var...")
  set(YAML_CPP_LIBRARIES yaml-cpp)
endif()

message(STATUS "Yaml-cpp headers: ${YAML_CPP_INCLUDE_DIR}")
message(STATUS "Yaml-cpp libraries: ${YAML_CPP_LIBRARIES}")

# Include PUBLICly so including projects get these as well
target_include_directories(psiact SYSTEM PRIVATE ${Boost_INCLUDE_DIRS} ${YAML_CPP_INCLUDE_DIR})
target_link_libraries(psiact PUBLIC ${Boost_LIBRARIES} ${YAML_CPP_LIBRARIES})
install(TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}_Targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
  ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

install(EXPORT ${PROJECT_NAME}_Targets
  FILE ${PROJECT_NAME}Targets.cmake
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${INSTALL_CONFIGDIR})

install(FILES
  ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
  ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${INSTALL_CONFIGDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/ DESTINATION
  ${CMAKE_INSTALL_INCLUDEDIR} PATTERN ".gitkeep" EXCLUDE)

# ############################################################################
#
# FIND OPTIONAL PACKAGES
#
# ############################################################################
MESSAGE(STATUS)
MESSAGE(STATUS "Searching for additional packages ------------------------")

FIND_PACKAGE(Doxygen)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Doxygen DEFAULT_MSG DOXYGEN_EXECUTABLE)

# Seems like the system includes do not need to be specified explicitly?
# If they are: Watch out as they might collide with whatever is provided by
# CMAKE_PREFIX_PATH.

# include_directories("/opt/local/include")
# link_directories("/opt/local/lib")
# PRINT LIBRARY AND INCLUDE SEARCH ORDER ON CONSOLE
# MESSAGE( STATUS "Search order for includes --------------------------------" )
# get_property(directories DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
# MESSAGE( STATUS "${directories}")
# MESSAGE( STATUS )
# MESSAGE( STATUS "Search order for libraries -------------------------------" )
# get_property(directories DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY LINK_DIRECTORIES)
# MESSAGE( STATUS "${directories}")

# ############################################################################
#
# DEFINITION OF PACKAGES AND TARGETS
#
# ############################################################################

# IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
# SET(CMAKE_INSTALL_PREFIX
# "${PSIACT_SOURCE_DIR}" CACHE PATH "PSI-ACT install prefix" FORCE
# )
# ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
OPTION(TESTING "Set to OFF to prevent compilation of unit tests." OFF)

MESSAGE(STATUS)
MESSAGE(STATUS "-- PSI-ACT Build Options ---------------------------------")
MESSAGE(STATUS "CMAKE_INSTALL_PREFIX   = ${CMAKE_INSTALL_PREFIX}")
MESSAGE(STATUS "CMAKE_PREFIX_PATH      = ${CMAKE_PREFIX_PATH}")
MESSAGE(STATUS "TESTING                = ${TESTING}")
MESSAGE(STATUS "CMAKE_BUILD_TYPE       = ${CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "Change a value with: cmake -D<VAR>=<VALUE>")
MESSAGE(STATUS "----------------------------------------------------------")
MESSAGE(STATUS)

# TODO: add switch to build examples
# add_subdirectory(examples)

# doc target
option(BUILD_DOC "Build the doxygen doc" ON)

if(BUILD_DOC)
  IF(DOXYGEN_FOUND)
    IF(NOT TARGET doc)
      ADD_CUSTOM_TARGET(doc COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    ELSE()
      ADD_CUSTOM_TARGET(doc-psiact COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
      ADD_DEPENDENCIES(doc doc-psiact)
    ENDIF()
  ENDIF()
ENDIF()

# ############################################################################
#
# TESTING
#
# ############################################################################
if(TESTING)
  IF(NOT TARGET gtest)
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
      INSTALL_COMMAND ""
    )

    # Get GTest source and binary directories from CMake project
    ExternalProject_Get_Property(psi-act-gtest SOURCE_DIR BINARY_DIR)

    # Create a libgtest target to be used as a dependency by test programs
    add_library(gtest IMPORTED STATIC GLOBAL)
    add_dependencies(gtest psi-act-gtest)

    # Set libgtest properties
    set_target_properties(gtest PROPERTIES
      "IMPORTED_LOCATION" "${BINARY_DIR}/lib/libgtest.a"
      "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
    )

    # Create a libgmock target to be used as a dependency by test programs
    add_library(gmock IMPORTED STATIC GLOBAL)
    add_dependencies(gmock psi-act-gtest)

    # Set libgmock properties
    set_target_properties(gmock PROPERTIES
      "IMPORTED_LOCATION" "${BINARY_DIR}/lib/libgmock.a"
      "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
    )

    set(_GTEST_INCLUDE_DIRS ${SOURCE_DIR}/googletest/include)
    set(_GMOCK_INCLUDE_DIRS ${SOURCE_DIR}/googlemock/include)
    set(GTEST_INCLUDE_DIRS ${_GTEST_INCLUDE_DIRS} ${_GMOCK_INCLUDE_DIRS})

    # directories must exist to use INTERFACE inlcude dirs, but they are only created at
    # build time. so we manually create at configure time to make cmake happy.
    file(MAKE_DIRECTORY ${_GTEST_INCLUDE_DIRS})
    file(MAKE_DIRECTORY ${_GMOCK_INCLUDE_DIRS})

    # I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
    target_include_directories(gtest INTERFACE ${GTEST_INCLUDE_DIRS})
  ENDIF()

  enable_testing()
  add_subdirectory(test)
ENDIF() # testing

# ############################################################################
#
# LINT
#
# ############################################################################
get_property(LINT_TARGETS GLOBAL PROPERTY LINT_TARGETS)

IF(NOT TARGET psi-act-lint)
  add_custom_target(psi-act-lint ALL
    DEPENDS ${LINT_TARGETS}
    VERBATIM)
ENDIF()

message(STATUS "-- psiact ------------------------------------------------")
message(STATUS "----------------------------------------------------------")
message(STATUS)
