#pragma once
#include "google/protobuf/service.h"
#include<memory>
#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include<muduo/net/InetAddress.h>
#include<unordered_map>
#include<string>
#include<google/protobuf/descriptor.h>
//服务发布类，专门提供rpc服务发布
class RpcProvider
{
public:
    //由protoc生成的rpc方法都会继承自service类，所以用基类指针接受
    void NotifyService(google::protobuf::Service *service);
    //启动rpc服务，开始提供远程调用服务
    void run();
private:

    struct ServiceInfo
    { 
        google::protobuf::Service *m_service; //保存服务对象
        std::unordered_map<std::string,const google::protobuf::MethodDescriptor*> m_methodMap;//保存服务中的方法
    };

    std::unordered_map<std::string,ServiceInfo> m_serviceInfoMap;//服务名 ：服务对象
    //eventloop
    muduo::net::EventLoop eventLoop;

    //新的socket连接回调
    void onConnection(const muduo::net::TcpConnectionPtr& conn);
    void onMessage(const muduo::net::TcpConnectionPtr& conn,muduo::net::Buffer* buffer,muduo::Timestamp time);
    //Closure回调
    void SendRpcResponse(const muduo::net::TcpConnectionPtr& conn,google::protobuf::Message* response);
};


