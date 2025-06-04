#include"rpcprovider.h"
#include<string>
#include"mrpcapplication.h"

void RpcProvider::NotifyService(google::protobuf::Service *service){

}

void RpcProvider::run(){
    std::string ip = MrpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    auto port = atoi(MrpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip,port);
    //创建tcpserver
    muduo::net::TcpServer server(&eventLoop,address,"RpcProvider");
    //绑定连接回调和读写回调
    server.setConnectionCallback([this](TcpCOnnectionPtr& tcpconnection){
        this->onConnection(tcpconnection);
    });
    server.setMessageCallback([this](const TcpConnectionPtr& tcp,Buffer* buf,Timestamp time){
        this->onMessage(tcp,buf,time);
    });
    //设置线程数量
    server.setThreadNum(4);
    //启动tcp监听
    server.start();
    eventLoop.loop();

}


void RpcProvider::onConnection(const TcpConnectionPtr&){

}

void RpcProvider::onMessage(const TcpConnectionPtr&,Buffer*,Timestamp){
    
}