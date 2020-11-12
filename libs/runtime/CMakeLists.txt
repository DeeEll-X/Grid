aux_source_directory(. runtime)

add_library(runtime ${runtime})
target_link_libraries(runtime glog)