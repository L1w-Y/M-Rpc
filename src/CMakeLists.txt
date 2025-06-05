aux_source_directory(. SRC_LIST)

add_library(mrpc ${SRC_LIST})

target_link_libraries(mrpc muduo_net muduo_base pthread)