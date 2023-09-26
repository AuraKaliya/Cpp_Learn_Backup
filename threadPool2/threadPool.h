#ifndef _THREADPOOL_H
#define _THREADPOOL_H
#include "taskQueue.h"

#include "pthread.h"
#include "stdlib.h"
#include "stdio.h"
#include <iostream>
#include "unistd.h"
#include <string.h>
#include <string>
// using ThreadPool=struct ThreadPool;
// using Func=void(*)(void* arg);

#pragma once

template<typename T>
class ThreadPool
{
public:

        ThreadPool(int minThreadCount, int maxThreadCount);
//创建线程池，初始化 用构造函数代替
//ThreadPool* CreateThreadPool(int queueSize,int minThreadCount,int maxThreadCount);

        ~ThreadPool();
//销毁线程池  结束类，用析构函数替代
//int DestroyThreadPool(ThreadPool* pool);

//为线程池添加任务
//void AddThreadToPool(ThreadPool* pool ,Func func,void* arg);
    void AddTask(Task<T> task);

//查询线程池的状态   --工作线程个数  --存活线程个数  
//int GetThreadPoolBusyNum(ThreadPool* pool);
//int GetThreadPoolNowNum(ThreadPool* pool);
 int GetBusyNum();
 int GetNowNum();


private:
//线程启动函数   --不需要暴露给用户   ---保护/私有
static void* worker(void* arg);  //工作线程的任务函数
static void* manager(void* arg);   //管理者线程的任务函数
//void threadExited(ThreadPool* pool);    //线程退出
void threadExited(); 



private:
    //任务队列  ---在对象内有相关管理的操作
    TaskQueue<T> * m_taskQueue;
    //
    pthread_t manangerID;   //管理者线程ID
    pthread_t *threadIDs;   //工作的线程ID

    //线程数量的配置信息
    int minNum;     //最小线程数
    int maxNum;     //最大线程数
    int busyNum;    //工作中线程数   --频繁变化的需要单独加锁
    int nowNum;     //现存活线程数
    int killNum;    //需要杀掉的线程数

    pthread_mutex_t mutexPool; //互斥锁整个的线程池--操作任务队列时队线程池的资源的访问
    //用容器----容量没有上限限制
    pthread_cond_t notEmpty;    //判断任务队列是否空

    int status; //线程池的状态
};



#endif