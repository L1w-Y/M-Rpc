#include"rpcprovider.h"
#include"mrpcapplication.h"


void RpcProvider::NotifyService(google::protobuf::Service *service){

    ServiceInfo service_info;
    service_info.m_service = service;
    //获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor *pserverDesc = service->GetDescriptor();
    //获取服务名
    const std::string& service_name = pserverDesc->name();
    //获取服务中方法的个数 
    int methodCount = pserverDesc->method_count();  

    for(int i=0;i<methodCount;++i){
        const google::protobuf::MethodDescriptor* pmethodDesc = pserverDesc->method(i);
        const std::string& method_name = pmethodDesc->name();
        service_info.m_methodMap.emplace(method_name, pmethodDesc);
        std::cout<<"method_name:"<<method_name<<std::endl;
    }
    m_serviceInfoMap.emplace(service_name, std::move(service_info));
    std::cout<<"service name:"<<service_name<<std::endl;
    
}

void RpcProvider::run(){
    std::string ip = MrpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    auto port = atoi(MrpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip,port);
    //创建tcpserver
    muduo::net::TcpServer server(&eventLoop,address,"RpcProvider");
    //绑定连接回调和读写回调
    server.setConnectionCallback([this](const muduo::net::TcpConnectionPtr& tcpconnection){
        this->onConnection(tcpconnection);
    });
    server.setMessageCallback([this](const muduo::net::TcpConnectionPtr& tcp,muduo::net::Buffer* buf,muduo::Timestamp time){
        this->onMessage(tcp,buf,time);
    });
    //设置线程数量
    server.setThreadNum(4);
    std::cout<<"RpcProvider start service ip:"<<ip<<" prot:"<<port<<std::endl;
    //启动tcp监听
    server.start();
    eventLoop.loop();

}


void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr&){

}

void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr&,muduo::net::Buffer*,muduo::Timestamp){
    
}