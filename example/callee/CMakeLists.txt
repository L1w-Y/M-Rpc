set(SRC_LIST userservice.cc ../user.pb.cc) 
add_executable(provider ${SRC_LIST})
target_link_libraries(provider mrpc protobuf)