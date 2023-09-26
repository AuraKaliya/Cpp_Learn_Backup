#include "taskQueue.h"

template <typename T>
TaskQueue<T>::TaskQueue()
{
    //初始化  --互斥锁
    pthread_mutex_init(&m_mutex,NULL);
}
template <typename T>
TaskQueue<T>::~TaskQueue()
{
    //销毁回收
    pthread_mutex_destroy(&m_mutex);
}
template <typename T>
void TaskQueue<T>::addTask(Task<T> task)
{
    pthread_mutex_lock(&m_mutex);
    m_taskQ.push(task);
    pthread_mutex_unlock(&m_mutex);
}

template <typename T>
Task<T> TaskQueue<T>::takeTask()
{

    Task<T> t;
    pthread_mutex_lock(&m_mutex);
    //判断控制
    if(!m_taskQ.empty())
    {
       
        t=m_taskQ.front();
         //对m_taskQ进行修改操作了  ---线程同步-加锁
        m_taskQ.pop();
    }
    pthread_mutex_unlock(&m_mutex);
    return t;
}
template <typename T>
void TaskQueue<T>::addTask(callback f, void *arg)
{
    Task<T> task(f,arg);
    pthread_mutex_lock(&m_mutex);
    //对m_taskQ进行修改操作了  ---线程同步-加锁
    m_taskQ.push(task);
    pthread_mutex_unlock(&m_mutex);
}
