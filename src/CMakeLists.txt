#aux_source_directory(. SRC_LIST)
set(SRC_LIST mrpcappliction.cc mrpcconfig.cc rpcheader.pb.cc rpcprovider.cc mrpcchannel.cc mrpccontroller.cc logger.cc zookeeperutil.cc)

add_library(mrpc ${SRC_LIST})

target_link_libraries(mrpc muduo_net muduo_base pthread zookeeper_mt) 