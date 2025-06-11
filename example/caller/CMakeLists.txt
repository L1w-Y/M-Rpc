set(SRC_LIST calluserservice.cc ../user.pb.cc) 
add_executable(comsumer ${SRC_LIST})
target_link_libraries(comsumer mrpc protobuf)