#include"rpcprovider.h"
#include"mrpcapplication.h"
#include"rpcheader.pb.h"


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

//新的socket连接回调
void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr& conn){
    if(!conn->connected()){
        conn->shutdown();
    }
}

/*
    框架内部通信的消息格式
          消息头       请求的服务名+方法+参数长度       参数
    |----------------|-------------------------|-------------|
     4字节                      
     代表第二段数据长度
*/
//已建立连接用户的读写回调
void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr& conn,muduo::net::Buffer* buffer,muduo::Timestamp time){
    //接收远程rpc请求的数据
    std::string recv_buf = buffer->retrieveAllAsString();

    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size,4,0);
    //根据header_size读取数据头的原始字节流，反序列化得到rpc请求的详细信息
    std::string rpc_header_str = recv_buf.substr(4,header_size);
    mrpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;

    if(rpcHeader.ParseFromString(rpc_header_str)){
        std::cout<<"反序列化成功"<<std::endl;
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    }
    else{
        std::cout<<"反序列化失败"<<std::endl;
        return;
    }
    //获取参数字节流
    std::string args_str = recv_buf.substr(4+header_size,args_size);

     // === 调试信息输出 ===
    std::cout << "====== RPC Header Info ======" << std::endl;
    std::cout << "服务名 (service_name): " << service_name << std::endl;
    std::cout << "方法名 (method_name): " << method_name << std::endl;
    std::cout << "参数大小 (args_size): " << args_size << std::endl;
    std::cout << "参数 (args_str): " << args_str << std::endl;
    std::cout << "=============================" << std::endl;


    //获取service对象和方法
    auto it =m_serviceInfoMap.find(service_name);
    if(it == m_serviceInfoMap.end()){
        std::cout<<service_name<<"is not exits"<<std::endl;
        return;
    }

    auto mit = it->second.m_methodMap.find(method_name);

    if(mit == it->second.m_methodMap.end()){
        std::cout<<service_name<<":"<<method_name<<"is not exits"<<std::endl;
        return;
    }

    google::protobuf::Service *service = it->second.m_service; //获取service对象
    const google::protobuf::MethodDescriptor *method = mit->second;//获取method对象

    //生成rpc方法调用的请求request和响应response参数
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();

    if(!request->ParseFromString(args_str))
    {  
        std::cout<<"request parse error content:"<<args_str<<std::endl;
    }  

    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    //给下面的method绑定Closure回调
    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider,
                                const muduo::net::TcpConnectionPtr&,
                                google::protobuf::Message*>(this,&RpcProvider::SendRpcResponse,conn,response);

    //调用当前rpc发布的方法
    //假设应用层 new UserService().Login(controller,request,response,done)
    service->CallMethod(method,nullptr,request,response,done);
}

//Closure的回调操作，用于序列化rpc 响应和网络发送
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn,google::protobuf::Message* response){
    std::string response_str;
    if(response->SerializeToString(&response_str)){
        conn->send(response_str);
        conn->shutdown();   
    }  
    else{
        std::cout<<"serialize response_str error"<<std::endl;
    }
    conn->shutdown();
}




