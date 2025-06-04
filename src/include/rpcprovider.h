#pragma once
#include "google/protobuf/service.h"
#include<memory>
#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include<muduo/net/InetAddress.h>
//服务发布类，专门提供rpc服务发布
class RpcProvider
{
public:
    //由protoc生成的rpc方法都会继承自service类，所以用基类指针接受
    void NotifyService(google::protobuf::Service *service);
    //启动rpc服务，开始提供远程调用服务
    void run();
private:

    //eventloop
    muduo::net::EventLoop eventLoop;

    //新的socket连接回调
    void onConnection(const TcpConnectionPtr&);
    void onMessage(const TcpConnectionPtr&,Buffer*,Timestamp);
};


