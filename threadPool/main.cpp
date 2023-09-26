#include "threadPool.h"
#include <iostream>
#include "pthread.h"
#include "stdlib.h"
#include "unistd.h"

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
   ThreadPool* pool= CreateThreadPool(3,10,100);
   for(int i=0 ;i<100 ;++i)
   {    
        int * num =(int*) malloc(sizeof(int));
        *num=i+100;
        AddThreadToPool(pool,taskFunc,num);
   }
   sleep(5);
   DestroyThreadPool(pool);
    

    return 0;
}