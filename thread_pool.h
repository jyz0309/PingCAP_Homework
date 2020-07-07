//
// Created by alkaid on 2020/7/7.
//

#ifndef PINGCAP_HOMEWORK_THREAD_POOL_H
#define PINGCAP_HOMEWORK_THREAD_POOL_H

#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>
#include <malloc.h>

const int THREADPOOL_INVALID = -1;
const int THREADPOOL_LOCK_FAILURE = -2;
const int THREADPOOL_QUEUE_FULL = -3;
const int THREADPOOL_SHUTDOWN = -4;
const int THREADPOOL_THREAD_FAILURE = -5;
const int THREADPOOL_GRACEFUL = 1;

const int MAX_THREADS = 1024;
const int MAX_QUEUE = 65535;

typedef enum
{
    immediate_shutdown = 1,
    graceful_shutdown = 2
}threadpool_shutdown_t;

typedef struct{
    void (*function)(void *); //函数指针
    void *argument; //函数参数
}threadpool_task_t;

//线程池结构体
struct threadpool_t{
    pthread_mutex_t lock; //锁
    pthread_cond_t notify;
    pthread_t *threads;
    threadpool_task_t *queue;
    int thread_count;
    int queue_size;
    int head;
    int tail;
    int count;
    int shutdown;
    int started;
};

threadpool_t *threadpool_create(int thread_count, int queue_sizes);
int threadpool_add(threadpool_t *pool, void (*function)(void *), void *argument);
int threadpool_destroy(threadpool_t *pool, int flags);
int threadpool_free(threadpool_t *pool);
static void *threadpool_thread(void *threadpool);

#endif //PINGCAP_HOMEWORK_THREAD_POOL_H
