project(PSIACT CXX)

# if no option is given, standard is release
IF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Release)
ENDIF(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS_RELEASE " -Wall -O2 -mavx")
set(CMAKE_CXX_FLAGS_DEBUG " -Wall -pg")

LIST(APPEND PSIACT_SRCS 
	Command.cpp
	Option.cpp
	CycleManager.cpp
	Option.cpp
	PriorityArbitration.cpp
	SequentialPlan.cpp
	Arbitrator.cpp
)


LIST(APPEND PSIACT_HEADERS
	Command.h
	Option.h
	CycleManager.h
	Option.h
	PriorityArbitration.h
	SequentialPlan.h
	Arbitrator.h
	Time.h
)

LIST(APPEND FILES
  ${PSIACT_SRCS} 
  ${PSIACT_HEADERS})

# define subgroups for XCode and other IDEs
source_group( Core FILES ${PSIACT_HEADERS} ${PSIACT_SRCS} )


ADD_LIBRARY(psiact STATIC ${FILES})

set_target_properties(psiact PROPERTIES COMPILE_FLAGS "-std=c++11")


INSTALL(FILES ${PSIACT_HEADERS} DESTINATION include)

INSTALL(TARGETS psiact
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# add_lint_target(psiact-library-lint "${FILES}")
# set_property(GLOBAL APPEND PROPERTY LINT_TARGETS psiact-library-lint)
