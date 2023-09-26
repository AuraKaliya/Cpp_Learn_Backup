#ifndef _THREADPOOL_H
#define _THREADPOOL_H

using ThreadPool=struct ThreadPool;
using Func=void(*)(void* arg);
//创建线程池，初始化
ThreadPool* CreateThreadPool(int queueSize,int minThreadCount,int maxThreadCount);

//销毁线程池
int DestroyThreadPool(ThreadPool* pool);

//为线程池添加任务
void AddThreadToPool(ThreadPool* pool ,Func func,void* arg);

//查询线程池的状态   --工作线程个数  --存活线程个数  
int GetThreadPoolBusyNum(ThreadPool* pool);
int GetThreadPoolNowNum(ThreadPool* pool);

//线程启动函数
void* worker(void* arg);
void* manager(void* arg);
void threadExited(ThreadPool* pool);
#endif