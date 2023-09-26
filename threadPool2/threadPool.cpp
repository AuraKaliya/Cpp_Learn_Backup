
#include "threadPool.h"
const int ADDNUM=2;
const int DELNUM=2;
// //任务结构体
// typedef struct Task<T>
// {
//     /* data */
//     Func function;
//     //存放一个函数指针表示执行的任务
//     void* arg;
//     //存放一个指针指向向任务传递的参数

// }Task<T>;


// //线程池结构体
// typedef struct ThreadPool
// {
//     /* data */
//     Task<T>* taskQueue;
//     //任务队列
//     int queueCapacity;  //容量
//     int queueSize;      //当前任务个数
//     int queueFront;     //队头-->取数据
//     int queueRear;      //队尾-->放数据

//     //
//     pthread_t manangerID;   //管理者线程ID
//     pthread_t *threadIDs;   //工作的线程ID

//     //线程数量的配置信息
//     int minNum;     //最小线程数
//     int maxNum;     //最大线程数
//     int busyNum;    //工作中线程数   --频繁变化的需要单独加锁
//     int nowNum;     //现存活线程数
//     int killNum;    //需要杀掉的线程数

//     pthread_mutex_t mutexPool; //互斥锁整个的线程池--操作任务队列时队线程池的资源的访问
//     pthread_mutex_t mutexBusy; //锁busy线程的资源
//     pthread_cond_t notFull;     //判断任务队列是否满
//     pthread_cond_t notEmpty;    //判断任务队列是否空

//     int status; //线程池的状态
// }ThreadPool;


template<typename T>
ThreadPool<T>::ThreadPool(int minThreadCount, int maxThreadCount)
{



    do{
            //对任务队列实例化
    m_taskQueue =new TaskQueue<T>;
    //ThreadPool* pool =(ThreadPool*)malloc(sizeof(ThreadPool));
    if(m_taskQueue==nullptr)
    {
         //任务队列创建失败
        std::cout<<"new m_taskQueue failed..."<<std::endl;
        break;
    }


    threadIDs=new pthread_t[maxThreadCount];
    if(threadIDs==nullptr)
    {
        //线程池创建失败
        std::cout<<"new threadIDs failed..."<<std::endl;
        break;
    }

    //对线程池管理的线程进行初始化内存分配
    //pool->threadIDs=(pthread_t*)malloc(sizeof(pthread_t)*maxThreadCount);
    // if(!pool)
    // {
    //     //线程创建失败
    //     std::cout<<"malloc threadIDs fail"<<std::endl;
    //     break;
    // }

    memset(threadIDs,0,sizeof(pthread_t)*maxThreadCount);
    //分配内存后进行内存的初始化

    // pool->minNum=minThreadCount;
    // pool->maxNum=maxThreadCount;
    // pool->busyNum=0;
    // pool->nowNum=minThreadCount;
    // pool->killNum=0;
    minNum=minThreadCount;
    maxNum=maxThreadCount;
    busyNum=0;
    nowNum=minThreadCount;
    killNum=0;


    if(pthread_mutex_init(&mutexPool,NULL)!=0 ||
        pthread_cond_init(&notEmpty,NULL)!=0 )
       {
        std::cout<<"init fail"<<std::endl;
       break;
       }

    // //任务队列初始化   ---在new时已经做过

    // pool->taskQueue=(Task<T>*)malloc(sizeof(Task<T>)*queueSize);
    // pool->queueCapacity=queueSize;
    // pool->queueSize=0;
    // pool->queueFront=0;
    // pool->queueRear=0;

    status=0;

    //创建线程
    //管理者线程   --①改为静态方法  ②放到外面并定义友元
    pthread_create(&manangerID,NULL,manager,this);
    //工作线程的函数（manager）
    for(int i=0;i<minThreadCount;++i)
    {
        pthread_create(&threadIDs[i],NULL,worker,this);
    }

    }while (0);

    // if(pool && pool->threadIDs) free(pool->threadIDs);
    // if(pool && pool->taskQueue) free(pool->taskQueue);
    // if(pool) free(pool);
    //先判断pool是否存在，再访问pool内部成员
    //出现异常，释放资源（适用于多出口且需要释放堆内存的情况）
    if(threadIDs) delete[] threadIDs;
    if(m_taskQueue)  delete m_taskQueue;

}


template<typename T>
ThreadPool<T>::~ThreadPool()
{
    //若指向有效地址，需要先关闭线程池
    status=true;
    pthread_join(manangerID,NULL);        //阻塞回收管理者线程  --阻塞等待管理者线程退出
    //唤醒阻塞的消费者线程（存活的线程）  --此时状态是线程池被销毁了 --所以被唤醒即被销毁
    for(int i=0;i<nowNum;++i)
    {
        pthread_cond_signal(&notEmpty);
    }

    //释放内存
    if(m_taskQueue)
    {
        delete m_taskQueue;
        //pool->taskQueue=NULL;
    }
    if(threadIDs)
    {
        delete[] threadIDs;
        //pool->threadIDs=NULL;
    }
    printf("NOW This complete!\n");
    pthread_mutex_destroy(&mutexPool);
    pthread_cond_destroy(&notEmpty);

}

template<typename T>
void* ThreadPool<T>::worker(void* arg)
{
    //在静态方法中调用实例对象---->传入void指针，强制类型转换。

    ThreadPool<T>* pool =static_cast<ThreadPool<T>*>(arg);
    while(1)
    {
        //进入循环读取线程池资源

        pthread_mutex_lock(&pool->mutexPool);
        printf("pool lock\n");
        //当前任务队列是否为空
        while(pool->m_taskQueue->taskNumber()==0 && !pool->status)
        {
            //当前任务队列为空且线程池子可使用
            //阻塞工作线程
            pthread_cond_wait(&pool->notEmpty,&pool->mutexPool);
            //进入一个条件等待状态，以pool->notEmpty的状态为监测对象，用于唤起pool->mutexPool线程。
            
            //被唤醒时执行的操作的控制：根据唤醒状态选择执行任务或销毁
            if(pool->killNum>0)
            {
                pool->killNum--; //这是唤醒后的控制变量，无论如何都会递减，表示的是已经对这部分处理了。
                if(pool->nowNum>pool->minNum)
                {
                    pool->nowNum--;
                    printf("pool unlock in wait to kill\n");
                    pthread_mutex_unlock(&pool->mutexPool);
                    pool->threadExited();
                }

            }
        }

        //判断线程池是否可使用
        if(pool->status)
        {
            printf("pool unlock in pool destoried\n");
            pthread_mutex_unlock(&pool->mutexPool);
            pool->threadExited();
        }

        // Task<T> task;
        // task.function = pool->taskQueue[pool->queueFront].function;
        // task.arg=pool->taskQueue[pool->queueFront].arg;
        // //读取之后列表节点向后移动
        // //维护为一个循环队列
        // pool->queueFront=(pool->queueFront+1)% pool->queueCapacity;
        // pool->queueSize--;

        Task<T> task=pool->m_taskQueue->takeTask();

        //取出了一个后，它不满了，唤醒生产者生产任务  ---去除了队列限制，不需要再进行“唤醒”
        //pthread_cond_signal(&pool->notFull);
      

        printf("thread %ld start working... \n",pthread_self());
        pool->busyNum++;
        printf("pool unlock\n");
        pthread_mutex_unlock(&pool->mutexPool);
        task.function(task.arg);
        //在函数调用时，如果传入的是堆内存，在未释放前一直存在，如果传入的是栈内存，则可能在某函数调用后被释放。
        //传入的是堆内存，需要手动释放
        //std::cout<<"thread end working..."<<std::endl;

       // delete (task.arg);
        task.arg=nullptr;

        printf("thread %ld end working... \n",pthread_self());
        pthread_mutex_lock(&pool->mutexPool);
        pool->busyNum--;
        pthread_mutex_unlock(&pool->mutexPool);

    }
    return NULL;
}
template<typename T>
void* ThreadPool< T>::manager(void* arg)
{
    //在绑定时传入的参数为pool，所以转换为ThreadPool*
    ThreadPool<T>* pool =static_cast<ThreadPool<T>*>(arg);

//需要进入一个循环执行的过程，以一定频率对线程池进行调节。
    while(!pool->status)
    {
            //每隔3s检测一次。
            sleep(3);
            //取出线程池中的任务的数量和当前线程数量
            pthread_mutex_lock(&pool->mutexPool);
            int queueSize=pool->m_taskQueue->taskNumber();
            int nowNum=pool->nowNum;
            //取出忙线程数量
            int busyNum=pool->busyNum;
            pthread_mutex_unlock(&pool->mutexPool);


            //添加和销毁线程
            //添加策略    --任务数>存在线程个数  && 存在线程个数<最大线程数
            if(queueSize>nowNum && nowNum<pool->maxNum)
            {
                pthread_mutex_lock(&pool->mutexPool);
                int count =0;
                //访问了线程池
                for(int i=0; i<pool->maxNum && count<ADDNUM &&pool->nowNum < pool->maxNum;++i)
                {
                    if(pool->threadIDs[i]==0)  //线程未被使用时=0
                    {
                      pthread_create(&pool->threadIDs[i],NULL,worker,pool);
                      count++;
                      pool->nowNum++;
                    }
                   
                }
                pthread_mutex_unlock(&pool->mutexPool);
            }
             
             //销毁策略  --忙线程远小于存活线程数 && 存活线程数>最小线程数
            if(busyNum* 2 < nowNum && nowNum>pool->minNum)
            {
                pthread_mutex_lock(&pool->mutexPool);
                pool->killNum =DELNUM;
                pthread_mutex_unlock(&pool->mutexPool);

                //让工作线程自杀
                // --明确状态：空闲线程处于阻塞的条件等待状态  --策略：唤醒线程并不分配任务，直接结束线程。
                for(int i=0;i<DELNUM;++i)
                {
                    pthread_cond_signal(&pool->notEmpty);
                }
            }

    }

    return NULL;
}

template<typename T>
void ThreadPool<T>::threadExited()
{
    pthread_t tid=pthread_self();
    for(int i=0;i<this->maxNum;++i)
    {
        printf("threadExited called %ld  ... now i=%d \n",tid,i);
        if(this->threadIDs[i]==tid)
        {
            this->threadIDs[i]=0;
            //std::cout<<"threadExited called,"<<tid<<" exiting"<<std::endl;
            printf("threadExited called %ld  exiting\n",tid);
            break;
        }
    }
    pthread_exit(NULL);
    //标准C的函数 所以用NULL
}
template<typename T>
void ThreadPool<T>::AddTask(Task<T> task)
{
    //任务队列中已经在维护一把互斥锁了，所以它的add操作是线程安全的，不需要进行重复的线程同步。
    //pthread_mutex_lock(&mutexPool);

    // //判断队列满  --队列上限已解除
    // while(pool->queueSize==pool->queueCapacity && !pool->status)
    // {
    //     //阻塞生产者线程
    //     pthread_cond_wait(&pool->notFull,&pool->mutexPool);
    // }

    if(status)
    {
        pthread_mutex_unlock(&mutexPool);
        return;
    }

    //添加任务
    // pool->taskQueue[pool->queueRear].function=func;
    // pool->taskQueue[pool->queueRear].arg=arg;
    // pool->queueRear=(pool->queueRear+1)%pool->queueCapacity;
    // pool->queueSize++;
    m_taskQueue->addTask(task);

    //唤醒工作线程取任务
    pthread_cond_signal(&notEmpty);

   // pthread_mutex_unlock(&mutexPool);
}
template<typename T>
int ThreadPool<T>::GetBusyNum()
{
    pthread_mutex_lock(&mutexPool);
    int busyNum=this ->busyNum;
    pthread_mutex_unlock(&mutexPool);
    return busyNum;
}
template<typename T>
int ThreadPool<T>::GetNowNum()
{
    pthread_mutex_lock(&mutexPool);
    int nowNum=this ->nowNum;
    pthread_mutex_unlock(&mutexPool);
    return nowNum;
}
