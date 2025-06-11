#include<iostream>
#include"mrpcapplication.h"
#include"user.pb.h"

int main(int argc,char **argv){
    MrpcApplication::Init(argc,argv);
    //模拟调用远程发布的rpc方法Login
    fixbug::UserServiceRpc_Stub stub(new MrpcChannel());
    fixbug::LoginRequest request;
    request.set_name("zhng");
    request.set_pwd("1234");
    fixbug::LoginResponse response;
    MrpcController controller;
    //底层调用的是构造stub时创建的RpcChannel::callMethod，在框架层面做的统一接口来调用到不同的rpc方法和网络发送
    stub.Login(&controller,&request,&response,nullptr);
    //rpc调用完成，读取结果
    if(controller.Failed())
    {
        std::cout<<controller.ErrorText()<<std::endl;
    }
    else
    {
        if(0==response.result().errcode())
        {
            std::cout<<"rpc login response:"<<response.success()<<std::endl;
        }
        else
        {
            std::cout<<"rpc login response error:"<<response.result().errmsg()<<std::endl;
        }
    }

    
    return 0;
}