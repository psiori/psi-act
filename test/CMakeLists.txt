file(GLOB SRCS *.cpp)

add_executable(test_psiact ${SRCS})

add_dependencies(test_psiact psiact)

target_link_libraries(test_psiact 
  libgtest
  libgmock
  psitest 
)


set_target_properties(test_psiact PROPERTIES COMPILE_FLAGS -std=c++11)

include_directories(
  ${PSIACT_SOURCE_DIR})

install(TARGETS test_psiact DESTINATION bin)

add_test(testPsiAct test_psiact)