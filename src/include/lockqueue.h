#pragma once

#include<queue>
#include<thread>
#include<mutex>
#include<condition_variable>

template<typename T>
class  LockQueue
{
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condvariable;
public:
    void Push(const T &data);
    T& Pop();
     LockQueue();
    ~ LockQueue();
};
