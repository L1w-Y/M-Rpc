#include"mrpcchannel.h"
#include"rpcheader.pb.h"
#include<errno.h>
#include <sys/types.h>     
#include <sys/socket.h>    
#include <netinet/in.h>     
#include <arpa/inet.h>    
#include <unistd.h>  
#include "mrpcapplication.h"   
void MrpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                    google::protobuf::RpcController* controller,
                    const google::protobuf::Message* request,
                    google::protobuf::Message* response,
                    google::protobuf::Closure* done)
{
    const google::protobuf::ServiceDescriptor* sd = method->service();
    std::string service_name = sd->name();
    std::string method_name = method->name();
    int args_size;
    std::string args_str;
    if(request->SerializeToString(&args_str)){
        args_size = args_str.size();
    }
    else{
        controller->SetFailed("serialize request error!");
        return;
    }

    mrpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);

    uint32_t header_size = 0;
    std::string rpc_header_str;
    if(rpcHeader.SerializeToString(&rpc_header_str)){
        header_size = rpc_header_str.size();
    }
    else{
        controller->SetFailed("serialize rpc header error!");
        return;
    }
    //组织待发送的rpc请求字符串
    std::string send_rpc_str;
    send_rpc_str.insert(0,std::string((char*)&header_size,4));
    send_rpc_str += rpc_header_str;
    send_rpc_str += args_str;
        std::cout << "====== RPC Header Info ======" << std::endl;
    std::cout << "发送方-服务名 (service_name): " << service_name << std::endl;
    std::cout << "发送方-方法名 (method_name): " << method_name << std::endl;
    std::cout << "发送方-参数大小 (args_size): " << args_size << std::endl;
    std::cout << "发送方-参数 (args_str): " << args_str << std::endl;
    std::cout << "=============================" << std::endl;

    int clientfd = socket(AF_INET,SOCK_STREAM,0);
    if(-1 == clientfd){
        std::cout<<"create socket error. errno:"<<errno<<std::endl;
        char errtxt[512] = {};
        sprintf(errtxt,"create socket error! errno:%d",errno);
        controller->SetFailed(errtxt);
        return;
    }
    std::string ip = MrpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MrpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    if(-1 == connect(clientfd,(struct sockaddr*)&server_addr,sizeof(server_addr))){
        std::cout<<"connect error. errno:"<<errno<<std::endl;
        char errtxt[512] = {};
        sprintf(errtxt,"connect error! errno:%d",errno);
        controller->SetFailed(errtxt);
        return;
    }
    if(-1 == send(clientfd,send_rpc_str.c_str(),send_rpc_str.size(),0)){
        std::cout<<"send error. errno:"<<errno<<std::endl;
        char errtxt[512] = {};
        sprintf(errtxt,"send error! errno:%d",errno);
        controller->SetFailed(errtxt);
        return;
    }
    //接收响应
    char recv_buf[1024]={};
    int recv_size = recv(clientfd,recv_buf,1024,0);
    if(-1 == recv_size){
        std::cout<<"recv error. errno:"<<errno<<std::endl;
        char errtxt[512] = {};
        sprintf(errtxt,"send error! errno:%d",errno);
        controller->SetFailed(errtxt);
        close(clientfd);    
        return;
    }

    std::string response_str(recv_buf, recv_size);
    //反序列化填入responses，上层用户访问response来获取结果
    if(!response->ParseFromString(response_str)){
        std::cout<<"parse error. response str:"<<response_str<<std::endl;
        char errtxt[512] = {};
        sprintf(errtxt,"parse error. response str:%s",response_str);
        controller->SetFailed(errtxt);
        return;
    }
    close(clientfd);
}