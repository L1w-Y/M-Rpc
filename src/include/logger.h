#pragma once
#include"lockqueue.h"
enum LogLevel
{
    INFO, //普通信息
    ERROR,//错误信息
};

//日志系统

class Logger
{
public:
    //设置日志级别
    void SetLogLevel(LogLevel level);
    //写日志
    void Log(std::string msg);

private:
    int m_loglevel;//记录日志级别
    LockQueue<std::string> m_lockQue;//日志缓冲队列   

    Logger();
    Logger(const Logger&)=delete;
    Logger(Logger&&)=delete;
};

