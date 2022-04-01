cmake_minimum_required(VERSION 3.15.0)

project(psiact CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# ############################################################################
#
#   FIND OPTIONAL PACKAGES
#
# ############################################################################

MESSAGE( STATUS )
MESSAGE( STATUS "Searching for additional packages ------------------------" )

FIND_PACKAGE(Doxygen)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Doxygen DEFAULT_MSG DOXYGEN_EXECUTABLE)


include_directories("/opt/local/include")
link_directories("/opt/local/lib")

# PRINT LIBRARY AND INCLUDE SEARCH ORDER ON CONSOLE

MESSAGE( STATUS )
MESSAGE( STATUS "Search order for includes --------------------------------" )
get_property(directories DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
MESSAGE( STATUS "${directories}")

MESSAGE( STATUS )
MESSAGE( STATUS "Search order for libraries -------------------------------" )
get_property(directories DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY LINK_DIRECTORIES)
MESSAGE( STATUS "${directories}")


# ############################################################################
#
#   HELPER FUNCTIONS USED IN CMAKEFILES
#
# ############################################################################

# the following styles and lint function will be used in the package
# definitions.

set(STYLE_FILTER)
set(STYLE_FILTER ${STYLE_FILTER}-legal/copyright,)
set(STYLE_FILTER ${STYLE_FILTER}-build/header_guard,)
set(STYLE_FILTER ${STYLE_FILTER}-build/include,)
set(STYLE_FILTER ${STYLE_FILTER}+build/include_order,)
set(STYLE_FILTER ${STYLE_FILTER}+build/include_what_you_use,)

FUNCTION(add_lint_target TARGET_NAME SOURCES_LIST)

  list(REMOVE_DUPLICATES SOURCES_LIST)
  list(SORT SOURCES_LIST)

  if(NOT ${TARGET_NAME})
  add_custom_target(${TARGET_NAME}
    COMMAND "${CMAKE_COMMAND}" -E chdir
            "${CMAKE_CURRENT_SOURCE_DIR}"
            "${PYTHON_EXECUTABLE}"
            "${CMAKE_SOURCE_DIR}/vendor/cpplint.py"
            "--filter=${STYLE_FILTER}"
            "--counting=detailed"
            "--extensions=cc,cpp,hpp,h"
            "--linelength=80"
            ${SOURCES_LIST}
    DEPENDS ${SOURCES_LIST}
    COMMENT "Linting ${TARGET_NAME}"
    VERBATIM)
    endif()

endfunction()


# ############################################################################
#
#   DEFINITION OF PACKAGES AND TARGETS
#
# ############################################################################

# IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
# SET(CMAKE_INSTALL_PREFIX
# "${PSIACT_SOURCE_DIR}" CACHE PATH "PSI-ACT install prefix" FORCE
# )
# ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

OPTION( TESTING "Set to OFF to prevent compilation of unit tests." OFF )

MESSAGE( STATUS )
MESSAGE( STATUS "-- PSI-ACT Build Options ---------------------------------" )
MESSAGE( STATUS "CMAKE_INSTALL_PREFIX   = ${CMAKE_INSTALL_PREFIX}" )
MESSAGE( STATUS "TESTING                = ${TESTING}" )
MESSAGE( STATUS "CMAKE_BUILD_TYPE       = ${CMAKE_BUILD_TYPE}")
MESSAGE( STATUS "Change a value with: cmake -D<VAR>=<VALUE>" )
MESSAGE( STATUS "----------------------------------------------------------" )
MESSAGE( STATUS )

add_subdirectory(src)
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
#   TESTING
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
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/psi-act-gtest
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
#   LINT
#
# ############################################################################

get_property(LINT_TARGETS GLOBAL PROPERTY LINT_TARGETS)

IF(NOT TARGET psi-act-lint)
add_custom_target(psi-act-lint ALL
  DEPENDS ${LINT_TARGETS}
  VERBATIM)
ENDIF()
