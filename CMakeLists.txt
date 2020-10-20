# 项目信息
PROJECT(Grid)
cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 ")
set(CMAKE_BUILD_TYPE "Debug")

find_package(Boost 1.65.1 COMPONENTS program_options REQUIRED)
include_directories( ${Boost_INCLUDE_DIR})
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
link_libraries(${JSONCPP_LIBRARIES})
message(${JSONCPP_LIBRARIES})

add_subdirectory(binary)
add_subdirectory(externals)
add_subdirectory(libs)
add_subdirectory(tests)