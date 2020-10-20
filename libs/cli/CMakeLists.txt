aux_source_directory(. cli)

add_library(cli ${cli})
target_link_libraries(cli glog)