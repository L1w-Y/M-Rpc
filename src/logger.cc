#include"logger.h"
#include<time.h>
#include<iostream>
//获取日志单例
Logger& Logger::GetInstance(){
    static Logger logger;
    return logger;
}
//设置日志级别
void Logger::SetLogLevel(LogLevel level){
    m_loglevel = level;
}
//写日志
void Logger::Log(std::string msg){

    std::string level_str;
    switch (m_loglevel) {
        case INFO:
            level_str = "[INFO] ";
            break;
        case ERROR:
            level_str = "[ERROR] ";
            break;
        default:
            level_str = "[UNKNOWN] ";
            break;
    }

    msg = level_str + msg;
    m_lockQue.Push(msg);
}

Logger::Logger(){
    std::thread writeLogTask([&](){
        for(;;){
            //获取当前信息，取日志信息，写入响应文件中
            time_t now = time(nullptr);
            tm *nowtm = localtime(&now);

            char file_name[128];
            sprintf(file_name,"%d-%d-%d-log.txt",nowtm->tm_year+1900,nowtm->tm_mon+1,nowtm->tm_mday);

            FILE *pf=fopen(file_name,"a+");
            if(pf == nullptr){
                std::cout<<"logger file:"<<file_name<<"open error!"<<std::endl;
                exit(EXIT_FAILURE);
            }
            std::string msg = m_lockQue.Pop();

            char time_buf[128] = {0};
            sprintf(time_buf, "[%02d:%02d:%02d] ", nowtm->tm_hour, nowtm->tm_min, nowtm->tm_sec);

            std::string log_line = time_buf + msg + "\n";

            fputs(log_line.c_str(), pf);            
            fclose(pf);
        }
    });

    writeLogTask.detach();
}