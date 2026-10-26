LIST(APPEND PSIACT_SRCS
  ${CMAKE_CURRENT_LIST_DIR}/ClockProvider.cpp
  ${CMAKE_CURRENT_LIST_DIR}/SystemClock.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Command.cpp
  ${CMAKE_CURRENT_LIST_DIR}/CycleManager.cpp
  ${CMAKE_CURRENT_LIST_DIR}/arbitration/Option.cpp
  ${CMAKE_CURRENT_LIST_DIR}/arbitration/PriorityArbitration.cpp
  ${CMAKE_CURRENT_LIST_DIR}/arbitration/SequentialPlan.cpp
  ${CMAKE_CURRENT_LIST_DIR}/arbitration/Arbitrator.cpp
  ${CMAKE_CURRENT_LIST_DIR}/arbitration/FiniteStateArbitration.cpp
  ${CMAKE_CURRENT_LIST_DIR}/deliberation/Deliberator.cpp
  ${CMAKE_CURRENT_LIST_DIR}/deliberation/FiniteStateMachine.cpp
  ${CMAKE_CURRENT_LIST_DIR}/deliberation/FiniteStateDeliberation.cpp
)

LIST(APPEND PSIACT_HEADERS
  ${CMAKE_CURRENT_LIST_DIR}/ClockProvider.hpp
  ${CMAKE_CURRENT_LIST_DIR}/SystemClock.hpp
  ${CMAKE_CURRENT_LIST_DIR}/ClockProviderRegistry.hpp
  ${CMAKE_CURRENT_LIST_DIR}/Command.h
  ${CMAKE_CURRENT_LIST_DIR}/CycleManager.h
  ${CMAKE_CURRENT_LIST_DIR}/TimePoint.h
  ${CMAKE_CURRENT_LIST_DIR}/arbitration/Option.h
  ${CMAKE_CURRENT_LIST_DIR}/arbitration/PriorityArbitration.h
  ${CMAKE_CURRENT_LIST_DIR}/arbitration/SequentialPlan.h
  ${CMAKE_CURRENT_LIST_DIR}/arbitration/Arbitrator.h
  ${CMAKE_CURRENT_LIST_DIR}/arbitration/FiniteStateArbitration.hpp
  ${CMAKE_CURRENT_LIST_DIR}/deliberation/Deliberator.h
  ${CMAKE_CURRENT_LIST_DIR}/deliberation/FiniteStateMachine.h
  ${CMAKE_CURRENT_LIST_DIR}/deliberation/FiniteStateDeliberation.h
)

LIST(APPEND FILES
  ${PSIACT_SRCS}
  ${PSIACT_HEADERS})


# define subgroups for XCode and other IDEs
source_group( src FILES ${PSIACT_HEADERS} ${PSIACT_SRCS} )

find_package(Boost)
message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
message(STATUS "Boost library dirs: ${Boost_LIBRARY_DIRS}")
ADD_LIBRARY(psiact STATIC ${PSIACT_SRCS})
target_compile_features(psiact PUBLIC cxx_std_17)
find_package(Threads REQUIRED)
target_link_libraries(psiact PUBLIC Threads::Threads)

# disable flags on msvc
if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(psiact PRIVATE -Wno-error -Wno-error=old-style-cast)
endif()

message(STATUS "PSIACT BOOST: ${Boost_LIBRARIES} ${Boost_INCLUDE_DIRS}")
# Include PUBLICly so including projects get these as well
target_include_directories(psiact SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(psiact PUBLIC ${Boost_LIBRARIES})

# Install headers into include/psiact/*, keeping the src directory structure.
# This matches what other libraries (opencv, tensorflow..) do, although they
# already maintain the explicit include/LIBRARY_NAME structure in their source
# tree. We prefer to have the headers next to the sources, so we need some
# mockery to still allow namespaced includes.
set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_PREFIX}/include)
set(CMAKE_BUILD_INCLUDEDIR ${CMAKE_BINARY_DIR}/include)
foreach(file ${PSIACT_HEADERS})
  set_target_properties(psiact PROPERTIES PUBLIC_HEADER ${file})
  get_filename_component(dir ${file} DIRECTORY BASE_DIR ${CMAKE_CURRENT_LIST_DIR})
  string(REGEX REPLACE "^${CMAKE_CURRENT_LIST_DIR}" "" dir "${dir}")
  string(REGEX REPLACE "^/" "" dir "${dir}")

  # During install, all headers are moved to a include/psiact subdirectory,
  # namespacing them away from other libraries to avoid conflicts.
  install(FILES ${file} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/psiact/${dir})

  # During build time we create the include/psiact/**/*.h structure within
  # the ./build directory, such that it resembles what we have after an install
  # without the actual need to move the headers out of the ./src directory.
  # message(STATUS "Copy ${file} to ${CMAKE_BUILD_INCLUDEDIR}/psiact/${dir}")
  file(MAKE_DIRECTORY ${CMAKE_BUILD_INCLUDEDIR}/psiact/${dir})
  configure_file(${file} ${CMAKE_BUILD_INCLUDEDIR}/psiact/${dir} COPYONLY)
endforeach()

# Only use the mocked ./build/include dir as include directory. CMake will take
# care of always moving the up-to-date headers there.
target_include_directories(psiact PUBLIC ${CMAKE_BUILD_INCLUDEDIR})


INSTALL(TARGETS psiact
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
message(STATUS "psiact CMAKE_INSTALL_INCLUDEDIR: ${CMAKE_INSTALL_INCLUDEDIR}")
