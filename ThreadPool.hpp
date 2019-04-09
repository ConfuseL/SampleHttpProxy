#ifndef THREADPOOL_HPP_INCLUDED
#define THREADPOOL_HPP_INCLUDED
#include"Condition.hpp"
#include<queue>
#include<iostream>
//参考他人博客，仅再度抽象封装
//原地址 https://www.cnblogs.com/yangang92/p/5485868.html
typedef struct task
{
    void *(*job)(void *args);
    void *arg;
}Task;
void * ThreadManager(void *arg);
class ThreadPool
{
    public:
    Condition status;
    std::queue<Task* > taskQueue;
    int total;
    int idleNum;
    int maxNum;
    bool quit;
    ThreadPool(int threads):total(0),idleNum(0),maxNum(threads),quit(false)
    {
        status.Init();
    }
    ~ThreadPool()
    {
        if(!quit)
        {
            status.Lock();
            quit=true;
            if(total>0)
            {
                status.WakeUpAll();
            }
            while(total)
            {
                status.Wait();
            }
        }
        status.UnLock();
        status.Destroy();
    }
    void Add( void *(*job)(void *args),void *arg)
    {
        Task *newTask= (Task *)malloc(sizeof(Task));
        newTask->job=job;
        newTask->arg=arg;
        status.Lock();
        taskQueue.push(newTask);
        if(idleNum>0)
        {
            status.WakeUp();
        }
        else if(total<maxNum)
        {
            pthread_t newId;
            pthread_create(&newId,NULL,ThreadManager,this);
            total++;
        }
        status.UnLock();
    }
};
    void * ThreadManager(void *arg)
    {
        struct timespec _time;
        bool timeout;
       // std::cout<<(int)pthread_self()<<"线程开始"<<std::endl;
        ThreadPool *pool=(ThreadPool *)arg;
        while(1)
        {
            timeout=false;
            pool->status.Lock();
            pool->idleNum++;
            while(pool->taskQueue.empty()&&!pool->quit)
            {
                // std::cout<<(int)pthread_self()<<"线程正在等待任务分配"<<std::endl;
                 clock_gettime(CLOCK_REALTIME,&_time);
                 _time.tv_sec+=2;
                 int status=pool->status.WaitSomeTime(&_time);
                 if(status==ETIMEDOUT)
                 {
               //std:: cout<<(int)pthread_self()<<"线程等待超时"<<std::endl;
                timeout=true;
                break;
                 }
            }
            pool->idleNum--;
            if(!pool->taskQueue.empty())
            {
                Task *t=pool->taskQueue.front();
                pool->taskQueue.pop();
                pool->status.UnLock();
                t->job(t->arg);
                free(t);
                pool->status.Lock();
            }
            if(pool->taskQueue.empty()&&pool->quit)
            {
                if((--(pool->total))==0)
                {
                    pool->status.WakeUp();
                }
                pool->status.UnLock();
                break;
            }
            if(timeout)
            {
                pool->total--;
                 pool->status.UnLock();
                 break;
            }
             pool->status.UnLock();
        }
       // std::cout<<(int)pthread_self()<<"线程结束工作"<<std::endl;
        return NULL;
    }
#endif // THREADPOOL_HPP_INCLUDED
