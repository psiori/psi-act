# caution: whenever adding files, need to reconfigure w/o cache, not just rebuild
file(GLOB SRCS *.cpp)

add_executable(test_psiact ${SRCS})

add_dependencies(test_psiact psiact)

target_link_libraries(test_psiact
  psiact gtest gmock
)

set_target_properties(test_psiact PROPERTIES COMPILE_FLAGS -std=c++20)

include_directories(
  ${PSIACT_SOURCE_DIR})

install(TARGETS test_psiact DESTINATION bin)

add_test(testPsiAct test_psiact)
