#pragma once

class MrpcApplication
{
public:
    static void Init(int argc,char **argv);
    static MrpcApplication& GetInstance();
private:
    MrpcApplication(){}
    MrpcApplication(const MrpcApplication&) = delete;
    MrpcApplication(const MrpcApplication&&) = delete;
}