#pragma once

#include <stdbool.h>
#include <pthread.h>

#include "job_queue.h"
#include "thread.h"

struct ThreadPool {
    struct Thread **threads;
    int thread_num;

    volatile int alive_threads;
    volatile int working_threads;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    struct JobQueue *job_queue;
    volatile bool alive;
};

struct ThreadPool *ThreadPoolCreate(int num_threads);
bool ThreadPoolAddTask(struct ThreadPool *pool, void (*func)(void *), void *arg);
void ThreadPoolWait(struct ThreadPool *pool);
void ThreadPoolDestory(struct ThreadPool *pool);