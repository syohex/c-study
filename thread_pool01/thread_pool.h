#pragma once

#include <stdbool.h>
#include <pthread.h>

typedef void (*ThreadFunc)(void *arg);

struct WorkerTask;
struct WorkerTask {
    ThreadFunc func;
    void *arg;
    struct WorkerTask *next;
};

struct ThreadPool {
    struct WorkerTask *task_list;
    pthread_mutex_t mutex;
    pthread_cond_t task_list_cond;
    pthread_cond_t finish_cond;
    volatile size_t working_count;
    volatile size_t thread_count;
    volatile bool running;
};

struct ThreadPool *ThreadPoolCreate(size_t num);
void ThreadPoolDestory(struct ThreadPool *tp);
bool ThreadPoolAddTask(struct ThreadPool *tp, ThreadFunc func, void *arg);
void ThreadPoolWait(struct ThreadPool *tp);