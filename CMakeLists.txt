# 项目信息
PROJECT(Grid)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_BUILD_TYPE "Debug")

add_subdirectory(binary)
add_subdirectory(externals)
add_subdirectory(libs)
add_subdirectory(tests)