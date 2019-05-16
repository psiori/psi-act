CMAKE_MINIMUM_REQUIRED(VERSION 2.6.3)

project(PSIACT CXX C)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 11)


# INCLUDE( configuration.cmake.in )

# ############################################################################
#
#   CHECK CUSTOM LIBRARY PATHS
#
# ############################################################################

# NOTE: ADD CUSTOM PATHS FIRST! BECAUSE THEY SHOULD BE SEARCHED AND USED FIRST

MESSAGE( STATUS )
MESSAGE( STATUS "Checking custom paths specified on command line ---------" )

FUNCTION( custom_path_prefix LIBRARY_NAME VARIABLE_NAME RELATIVE_PATH_TO_CHECK )
  set(${VARIABLE_NAME} ""
      CACHE FILEPATH "Custom PATH_PREFIX for ${LIBRARY_NAME}"
      PARENT_SCOPE)

  if(${VARIABLE_NAME})
    if(EXISTS "${${VARIABLE_NAME}}/${RELATIVE_PATH_TO_CHECK}")
      message ( STATUS "Found ${LIBRARY_NAME}: ${${VARIABLE_NAME}}")
      include_directories("${${VARIABLE_NAME}}/include")
      link_directories("${${VARIABLE_NAME}}/lib")
    else()
      message(SEND_ERROR "${LIBRARY_NAME} not found: can't find ${RELATIVE_PATH_TO_CHECK} in ${${VARIABLE_NAME}}")
    endif()
  endif()
ENDFUNCTION()

# you can use the following optional parameters to really have each
# library installed in a completely different path.


# ############################################################################
#
#   FIND OPTIONAL PACKAGES
#
# ############################################################################

MESSAGE( STATUS )
MESSAGE( STATUS "Searching for additional packages ------------------------" )

FIND_PACKAGE(Doxygen)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Doxygen DEFAULT_MSG DOXYGEN_EXECUTABLE)
FIND_PACKAGE(PythonInterp)


include_directories("/opt/local/include")
link_directories("/opt/local/lib")

include_directories("/usr/local/include")
link_directories("/usr/local/lib")


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

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
SET(CMAKE_INSTALL_PREFIX
"${PSIACT_SOURCE_DIR}" CACHE PATH "PSI-ACT install prefix" FORCE
)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

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

# doc target
IF(DOXYGEN_FOUND)
    IF(NOT TARGET doc)
    ADD_CUSTOM_TARGET(doc COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
	                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    ENDIF()
ENDIF()

# ############################################################################
#
#   TESTING
#
# ############################################################################

if(TESTING)
IF(NOT TARGET libgtest)
# We need thread support
find_package(Threads REQUIRED)

# Enable ExternalProject CMake module
include(ExternalProject)

# Download and install GoogleTest
ExternalProject_Add(
    gtest
    URL https://github.com/google/googletest/archive/master.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
    # Disable install step
    INSTALL_COMMAND ""
)

# Get GTest source and binary directories from CMake project
ExternalProject_Get_Property(gtest source_dir binary_dir)

# Create a libgtest target to be used as a dependency by test programs
add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest)

# Set libgtest properties
set_target_properties(libgtest PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/googlemock/gtest/libgtest.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

# Create a libgmock target to be used as a dependency by test programs
add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gtest)


# Set libgmock properties
set_target_properties(libgmock PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/googlemock/libgmock.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

# I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
include_directories("${source_dir}/googletest/include"
                    "${source_dir}/googlemock/include")

enable_testing()
add_subdirectory(test)

ENDIF()
ENDIF()


# ############################################################################
#
#   LINT
#
# ############################################################################

get_property(LINT_TARGETS GLOBAL PROPERTY LINT_TARGETS)

IF(NOT TARGET lint)
add_custom_target(lint ALL
  DEPENDS ${LINT_TARGETS}
  VERBATIM)
ENDIF()
