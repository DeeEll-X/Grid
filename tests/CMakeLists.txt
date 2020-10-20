aux_source_directory(. DIR_SRCS)
include_directories(../libs)
include(GoogleTest)

add_executable(unit_test ${DIR_SRCS})

target_link_libraries(unit_test PRIVATE gtest)
target_link_libraries(unit_test PRIVATE gmock)
target_link_libraries(unit_test PRIVATE runtime)
target_link_libraries(unit_test PRIVATE cli)

gtest_discover_tests(unit_test)