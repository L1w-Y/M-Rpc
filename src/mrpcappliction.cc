#include"mrpcapplication.h"
#include<iostream>
#include<string>
#include<unistd.h>
void ShowArgsHelp()
{
    std::cout<<"format: command -i <configfile>"<<std::endl;
}

MrpcConfig MrpcApplication::m_config;

void MrpcApplication::Init(int argc,char **argv){
    if(argc < 2){
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }
    int c = 0;
    std::string config_file;
    while((c = getopt(argc,argv,"i:"))!=-1){
        switch (c)
        {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            std::cout<<"invalid args"<<std::endl;
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
            std::cout<<"need config file"<<std::endl;
            ShowArgsHelp();
            exit(EXIT_FAILURE);   
        default:
            break;         
        }
    }

    //  加载配置文件
    m_config.LoadConfigFile(config_file.c_str());

     std::cout << "rpcserverip:" << m_config.Load("rpcserverip") << std::endl;
     std::cout << "rpcserverport:" << m_config.Load("rpcserverport") << std::endl;
     std::cout << "zookeeperip:" << m_config.Load("zookeeperip") << std::endl;
     std::cout << "zookeeperport:" << m_config.Load("zookeeperport") << std::endl;

}

MrpcConfig& MrpcApplication::GetConfig(){
    return m_config;
}

MrpcApplication& MrpcApplication::GetInstance(){
    static MrpcApplication app;
    return app;
}