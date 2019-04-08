#ifndef CONDITION_HPP_INCLUDED
#define CONDITION_HPP_INCLUDED
#include<pthread.h>
//参考他人博客，仅再度封装
//原地址 https://www.cnblogs.com/yangang92/p/5485868.html
class Condition
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
public:
    int Init()
    {
        int status;
        if((status=pthread_mutex_init(&mutex,NULL)))
            return status;
        if((status=pthread_cond_init(&cond,NULL)))
           return status;
        return 0;
    }
    int Lock()
    {
        return pthread_mutex_lock(&mutex);
    }
    int UnLock()
    {
        return pthread_mutex_unlock(&mutex);
    }
    int Wait()
    {
        return pthread_cond_wait(&cond,&mutex);
    }
    int WaitSomeTime(const struct timespec * wtime)
    {
        return pthread_cond_timedwait(&cond,&mutex,wtime);
    }
    int WakeUp()
    {
        return pthread_cond_signal(&cond);
    }
    int WakeUpAll()
    {
        return pthread_cond_broadcast(&cond);
    }
    int Destroy()
    {
        int status;
        if((status=pthread_mutex_destroy(&mutex)))
            return status;
        if((status=pthread_cond_destroy(&cond)))
           return status;
        return 0;
    }
};

#endif // CONDITION_HPP_INCLUDED
