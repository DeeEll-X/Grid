include_directories(../libs)

add_executable(Grid main.cc)

target_link_libraries(Grid cli runtime ${Boost_LIBRARIES} glog::glog)