//
// Created by alkaid on 2020/7/7.
//

//
// Created by alkaid on 2020/2/7.
//

//
// Created by alkaid on 2019/12/8.
//

#include "thread_pool.h"

//建立线程池
threadpool_t *threadpool_create(int thread_count, int queue_size)
{
    threadpool_t *pool;
    int i;
    //(void) flags;
    do
    {
        if(thread_count <= 0 || thread_count > MAX_THREADS || queue_size <= 0 || queue_size > MAX_QUEUE) {
            return NULL;
        }
        //建立线程池
        if((pool = (threadpool_t *)malloc(sizeof(threadpool_t))) == NULL)
        {
            break;
        }
        /* pool初始化 */
        pool->thread_count = 0;
        pool->queue_size = queue_size;
        pool->head = pool->tail = pool->count = 0; //工作数量
        pool->shutdown = pool->started = 0;
        /* 初始化内存空间*/
        pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);
        pool->queue = (threadpool_task_t *)malloc(sizeof(threadpool_task_t) * queue_size);

        /* 初始化锁和条件变量 */
        if((pthread_mutex_init(&(pool->lock), NULL) != 0) ||
           (pthread_cond_init(&(pool->notify), NULL) != 0) ||
           (pool->threads == NULL) ||
           (pool->queue == NULL))
        {
            break;
        }
        //创建线程
        for(i = 0; i < thread_count; i++) {
            if(pthread_create(&(pool->threads[i]), NULL, threadpool_thread, (void*)pool) != 0)
            {
                threadpool_destroy(pool, 0);
                return NULL;
            }
            pool->thread_count++;
            pool->started++;
        }
        return pool;
    }while(false);

    if (pool != NULL)
    {
        threadpool_free(pool);
    }
    return NULL;
}

//将工作添加进线程池的任务队列中
int threadpool_add(threadpool_t *pool, void (*function)(void *), void *argument)
{
    //printf("add to thread pool !\n");
    int err = 0;
    int next;
    //(void) flags;
    if(pool == NULL || function == NULL)
    {
        return THREADPOOL_INVALID;
    }
    // 往任务队列中添加任务时加锁
    if(pthread_mutex_lock(&(pool->lock)) != 0)
    {
        return THREADPOOL_LOCK_FAILURE;
    }
    next = (pool->tail + 1) % pool->queue_size;
    do
    {
        /* 检测是否线程池队列已满*/
        if(pool->count == pool->queue_size) {
            err = THREADPOOL_QUEUE_FULL;
            break;
        }
        /*检测是否正在关闭*/
        if(pool->shutdown) {
            err = THREADPOOL_SHUTDOWN;
            break;
        }
        /* 添加工作 */
        pool->queue[pool->tail].function = function;
        pool->queue[pool->tail].argument = argument;
        pool->tail = next;
        pool->count += 1;

        /* 检查锁*/
        if(pthread_cond_signal(&(pool->notify)) != 0) {
            err = THREADPOOL_LOCK_FAILURE;
            break;
        }
    } while(false);
    //解锁
    if(pthread_mutex_unlock(&pool->lock) != 0) {
        err = THREADPOOL_LOCK_FAILURE;
    }
    return err;
}

static void *threadpool_thread(void *threadpool)
{
    threadpool_t *pool = (threadpool_t *)threadpool;
    threadpool_task_t task;
    for(;;)
    {
        /* Lock must be taken to wait on conditional variable */
        pthread_mutex_lock(&(pool->lock)); //读锁

        /* Wait on condition variable, check for spurious wakeups.
           When returning from pthread_cond_wait(), we own the lock. */
        while((pool->count == 0) && (!pool->shutdown)) //在这里等待有请求的到达
        {
            pthread_cond_wait(&(pool->notify), &(pool->lock));
        }
        if((pool->shutdown == immediate_shutdown) ||
           ((pool->shutdown == graceful_shutdown) &&
            (pool->count == 0)))
        {
            break; //队列中task数目为0，break
        }
        /* Grab our task */
        task.function = pool->queue[pool->head].function; //获取头节点对应的task
        task.argument = pool->queue[pool->head].argument; //获取头结点对应的task参数
        pool->head = (pool->head + 1) % pool->queue_size; // 这个头结点已经被一个线程所占有，头结点往后移动
        pool->count -= 1; //队列中的task数目--
        /* Unlock */
        pthread_mutex_unlock(&(pool->lock));
        /* 开始工作 */
        (*(task.function))(task.argument);
    }
    --pool->started;
    pthread_mutex_unlock(&(pool->lock));
    pthread_exit(NULL);
    return(NULL);
}
//摧毁线程池
int threadpool_destroy(threadpool_t *pool, int flags)
{
    printf("Thread pool destroy !\n");
    int i, err = 0;

    if(pool == NULL)
    {
        return THREADPOOL_INVALID;
    }

    if(pthread_mutex_lock(&(pool->lock)) != 0)
    {
        return THREADPOOL_LOCK_FAILURE;
    }

    do
    {
        /* Already shutting down */
        if(pool->shutdown) {
            err = THREADPOOL_SHUTDOWN;
            break;
        }

        pool->shutdown = (flags & THREADPOOL_GRACEFUL) ?
                         graceful_shutdown : immediate_shutdown;

        /* Wake up all worker threads */
        if((pthread_cond_broadcast(&(pool->notify)) != 0) ||
           (pthread_mutex_unlock(&(pool->lock)) != 0)) {
            err = THREADPOOL_LOCK_FAILURE;
            break;
        }

        /* Join all worker thread */
        for(i = 0; i < pool->thread_count; ++i)
        {
            if(pthread_join(pool->threads[i], NULL) != 0)
            {
                err = THREADPOOL_THREAD_FAILURE;
            }
        }
    } while(false);

    /* Only if everything went well do we deallocate the pool */
    if(!err)
    {
        threadpool_free(pool);
    }
    return err;
}

//释放内存
int threadpool_free(threadpool_t *
/* Did we manage to allocate ? */pool)
{
    if(pool == NULL || pool->started > 0)
    {
        return -1;
    }
    if(pool->threads)
    {
        free(pool->threads);
        free(pool->queue);

        /* Because we allocate pool->threads after initializing the
           mutex and condition variable, we're sure they're
           initialized. Let's lock the mutex just in case. */
        pthread_mutex_lock(&(pool->lock));
        pthread_mutex_destroy(&(pool->lock));
        pthread_cond_destroy(&(pool->notify));
    }
    free(pool);
    return 0;
}
