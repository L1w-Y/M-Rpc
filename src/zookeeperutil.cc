#include"zookeeperutil.h"


void global_watcher(zhandle_t *zh,int type,
                    int state, const char* path,void *watcherCtx)
                {
                     if(type == ZOO_SESSION_EVENT){
                        if(state == ZOO_CONNECTED_STATE){
                            sem_t *sem =(sem_t*)zoo_get_context(zh);
                            sem_post(sem);
                        }
                     }
                }

ZkClient::ZkClient():m_zhandle(nullptr){
    
}
ZkClient::~ZkClient(){
    if(m_zhandle!=nullptr){
        zookeeper_close(m_zhandle);
    }
}

void ZkClient::Start(){
    std::string host = MrpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    std::string port = MrpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    std::string connstr=host+":"+port;
    /*
        zookeeper API客户端提供了三个线程
        1.API调用线程（当前线程）
        2.网络io线程 专门开了一个线程，用poll做网络io的请求
        3.watcher回调线程
    */
    //这个个异步连接
    m_zhandle = zookeeper_init(connstr.c_str(),global_watcher,30000,nullptr,nullptr,0);
    if(m_zhandle == nullptr){
        std::cout<<"zookeeper_init error"<<std::endl;
        exit(EXIT_FAILURE);
    }
    sem_t sem;
    sem_init(&sem,0,0);
    zoo_set_context(m_zhandle,&sem);

    sem_wait(&sem);
    std::cout<<"zookeeper_init success"<<std::endl;
}

void ZkClient::Create(const char *path,const char *data,int datalen,int state){
    char path_buffer[128];
    int bufferlen=sizeof(path_buffer);
    int flag;
    flag = zoo_exists(m_zhandle,path,0,nullptr);
    if(ZNONODE==flag){ //表示path的znode不存在
        //创建指定节点
        flag=zoo_create(m_zhandle,path,data,datalen,
        &ZOO_OPEN_ACL_UNSAFE,state,path_buffer,bufferlen);
        if(flag==ZOK)
        {
            std::cout<<"znode create success..path:"<<path<<std::endl;
        }
        else{
            std::cout<<"flag:"<<flag<<std::endl;
            std::cout<<"znode create error..path:"<<path<<std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

std::string ZkClient::GetData(const char *path){
    char buffer[64];
    int bufferlen = sizeof(buffer);
    int flag = zoo_get(m_zhandle,path,0,buffer,&bufferlen,nullptr);
    if(flag !=ZOK){
        std::cout<<"get znode error..path:"<<path<<std::endl;
        return "";
    }
    else{
        return buffer;
    }
}