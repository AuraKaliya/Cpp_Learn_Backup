
#include <iostream>
#include <queue>
#include <vector>
#include <pthread.h>
#pragma once
using callback=void(*)(void* arg);
//任务结构体

template <typename T> struct Task{
     public: 
     Task() { 
     function =nullptr; 
     arg=nullptr; 
     };
      Task(callback f,void* arg) 
      { 
        this->arg=(T*)arg; 
      function=f;
       }; 
       callback function; //存放一个函数指针表示执行的任务 
       T* arg; //存放一个指针指向向任务传递的参数 
       };

template <typename T>
class TaskQueue
{
public:
    TaskQueue();
    ~TaskQueue();
    //添加任务
    void addTask(Task<T> task);
    void addTask(callback f,void* arg); 
    //取出任务
    Task<T> takeTask(); 

    //查询当前任务个数   --代码替换比压栈更高效
    inline int taskNumber()
    {
        return m_taskQ.size();
    }

    //处于多线程下，需要添加互斥锁规范操作



private:
    std::queue<Task<T> > m_taskQ;
    pthread_mutex_t m_mutex;

};
