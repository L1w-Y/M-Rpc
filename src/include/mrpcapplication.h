#pragma once
#include "mrpcconfig.h"
class MrpcApplication
{
public:
    //  框架初始化操作 provider -i config.
    static void Init(int argc,char **argv);
    static MrpcApplication& GetInstance();
    static MrpcConfig& GetConfig();
private:

    static MrpcConfig m_config;

    MrpcApplication(){}
    MrpcApplication(const MrpcApplication&) = delete;
    MrpcApplication(const MrpcApplication&&) = delete;

};