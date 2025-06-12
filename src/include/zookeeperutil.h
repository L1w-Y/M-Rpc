#pragma once 
#include"mrpcapplication.h"
#include <semaphore.h>
#include<zookeeper/zookeeper.h>
#include<string>
//zookeeper客户端
class ZkClient
{
public:
    ZkClient();
    ~ZkClient();
    //zkclient启动连接zkserver
    void Start();
    //在zkserver上根据指定的path创建znode节点
    void Create(const char *path,const char *data,int datalen,int state=0);
    //获取znode节点的值
    std::string GetData(const char *path);
private:
    zhandle_t *m_zhandle;

};