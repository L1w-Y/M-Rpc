#include <iostream>
#include<string>
#include"user.pb.h"

class UserService :public fixbug::UserServiceRpc
{
public:
    bool Login(std::string name,std::string pwd){
        std::cout<<"doing local service:Login"<<std::endl;
        std::cout<<"name:"<<name<<"pwd"<<pwd<<std::endl;
        return true;
    }

    /*
    重写基类UserServiceRpc的虚函数，作为rpc服务提供方，当远端发起调用请求时，首先是来到rpc框架所提供的函数，来匹配本地需要做的业务，在执行完成后
    将结果重新序列化然后返回给远端
    1. caller ==> Login(LoginRequest) ==>muduo => callee
    2. callee ==> Login(LoginRequest) ==>
    */
    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done)
            {   
                //  应用获取相应的请求数据
                std::string name = request->name();
                std::string pwd = request->pwd();

                //执行本地业务
                bool login_result = Login(name,pwd);
                //写入响应，错误信息和返回值
                fixbug::ResultCode *code = response->mutable_result();
                code->set_errcode(0);
                code->set_errmsg("");
                response->set_success(login_result);
                //执行回调 执行序列化和网络发送
                done->Run();
            }

};

