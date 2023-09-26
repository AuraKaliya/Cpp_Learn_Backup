#include "threadPool.h"
#include "threadPool.cpp"
#include "taskQueue.h"
#include "taskQueue.cpp"
#include <iostream>
#include "pthread.h"
#include "stdlib.h"
#include "unistd.h"
template class ThreadPool<int>;

//模板类使用要包含源文件 //或把实现写在头文件中 //或显示声明
void taskFunc(void* arg)
{
    int num=*(int*)arg;
    // std::cout<<"thread "<<pthread_self()<<" is working,Number= "<<num<<std::endl;
    //printf("thread %ldis working,Number= %d \n",pthread_self(),num);
    //usleep(1000);
}


int main()
{
   std::cout.tie(0);

   ThreadPool<int> pool(3,10);
   for(int i=0 ;i<100 ;++i)
   {    
        int * num =new int(i+100);
        pool.AddTask(Task<int>(taskFunc,num));
   }
   sleep(5);

    return 0;
}