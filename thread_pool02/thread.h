#pragma once

#include <pthread.h>

struct ThreadPool;
struct Thread {
    int id;
    pthread_t thread;
    struct ThreadPool *pool;
};

struct Thread *ThreadCreate(struct ThreadPool *pool, int id, void *(*func)(void *));
void ThreadDestory(struct Thread *t);