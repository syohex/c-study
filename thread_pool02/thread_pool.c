#include "thread_pool.h"

#include <stdio.h>
#include <stdlib.h>

static void *Worker(void *arg) {
    struct Thread *t = arg;

    struct ThreadPool *pool = t->pool;

    pthread_mutex_lock(&pool->mutex);
    pool->alive_threads += 1;
    pthread_mutex_unlock(&pool->mutex);

    while (pool->alive) {
        BinarySemaphoreWait(pool->job_queue->has_job);

        if (pool->alive) {
            pthread_mutex_lock(&pool->mutex);
            pool->working_threads += 1;
            pthread_mutex_unlock(&pool->mutex);

            struct Job *task = JobQueuePull(pool->job_queue);
            if (task != NULL) {
                task->func(task->arg);
                free(task);
            }

            pthread_mutex_lock(&pool->mutex);
            pool->working_threads -= 1;
            if (pool->working_threads == 0) {
                pthread_cond_signal(&pool->cond);
            }
            pthread_mutex_unlock(&pool->mutex);
        }
    }

    pthread_mutex_lock(&pool->mutex);
    pool->alive_threads -= 1;
    pthread_mutex_unlock(&pool->mutex);

    return NULL;
}

struct ThreadPool *ThreadPoolCreate(int num_threads) {
    struct ThreadPool *pool = malloc(sizeof(struct ThreadPool));
    if (pool == NULL) {
        return NULL;
    }

    pool->thread_num = num_threads;
    pool->alive = true;
    pool->alive_threads = 0;
    pool->working_threads = 0;

    pool->job_queue = JobQueueCreate();
    if (pool->job_queue == NULL) {
        free(pool);
        return NULL;
    }

    pool->threads = malloc(num_threads * sizeof(struct Thread *));
    if (pool->threads == NULL) {
        JobQueueDestory(pool->job_queue);
        free(pool);
        return NULL;
    }

    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->cond, NULL);

    for (int i = 0; i < num_threads; ++i) {
        pool->threads[i] = ThreadCreate(pool, i, Worker);
    }

    while (pool->alive_threads != num_threads) {
        ;
    }

    return pool;
}

bool ThreadPoolAddTask(struct ThreadPool *pool, void (*func)(void *), void *arg) {
    struct Job *task = malloc(sizeof(struct Job));
    if (task == NULL) {
        return false;
    }

    task->func = func;
    task->arg = arg;

    JobQueuePush(pool->job_queue, task);
    return true;
}

void ThreadPoolWait(struct ThreadPool *pool) {
    pthread_mutex_lock(&pool->mutex);
    while (pool->job_queue->length > 0 || pool->working_threads > 0) {
        pthread_cond_wait(&pool->cond, &pool->mutex);
    }
    pthread_mutex_unlock(&pool->mutex);
}

void ThreadPoolDestory(struct ThreadPool *pool) {
    pool->alive = false;

    while (pool->alive_threads > 0) {
        BinarySemaphorePostAll(pool->job_queue->has_job);
    }

    JobQueueDestory(pool->job_queue);

    for (int i = 0; i < pool->thread_num; ++i) {
        ThreadDestory(pool->threads[i]);
    }

    free(pool->threads);
    free(pool);
}