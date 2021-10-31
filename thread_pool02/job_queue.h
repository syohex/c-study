#pragma once

#include <stdint.h>
#include <pthread.h>

#include "semaphore.h"

struct Job;
struct Job {
    struct Job *next;
    void (*func)(void *arg);
    void *arg;
};

struct JobQueue {
    pthread_mutex_t mutex;
    struct Job *front;
    struct Job *last;
    struct BinarySemaphore *has_job;
    volatile size_t length;
};

struct JobQueue *JobQueueCreate();
void JobQueueClear(struct JobQueue *jq);
void JobQueueDestory(struct JobQueue *jq);
void JobQueuePush(struct JobQueue *jq, struct Job *job);
struct Job *JobQueuePull(struct JobQueue *jq);