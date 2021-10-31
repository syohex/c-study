#include "job_queue.h"

#include <stdlib.h>

struct JobQueue *JobQueueCreate() {
    struct JobQueue *jq = malloc(sizeof(struct JobQueue));
    if (jq == NULL) {
        return NULL;
    }

    jq->front = NULL;
    jq->last = NULL;
    jq->length = 0;

    jq->has_job = BinarySemaphoreCreate();
    if (jq->has_job == NULL) {
        return NULL;
    }

    pthread_mutex_init(&jq->mutex, NULL);
    return jq;
}

void JobQueueClear(struct JobQueue *jq) {
    while (jq->length > 0) {
        struct Job *j = JobQueuePull(jq);
        free(j);
    }

    jq->front = NULL;
    jq->last = NULL;
    BinarySemaphoreReset(jq->has_job);
    jq->length = 0;
}

void JobQueueDestory(struct JobQueue *jq) {
    JobQueueClear(jq);
    BinarySemaphoreDestory(jq->has_job);
    pthread_mutex_destroy(&jq->mutex);
    free(jq);
}

void JobQueuePush(struct JobQueue *jq, struct Job *job) {
    pthread_mutex_lock(&jq->mutex);
    job->next = NULL;

    if (jq->length == 0) {
        jq->front = job;
        jq->last = job;
    } else {
        jq->last->next = job;
        jq->last = job;
    }
    jq->length += 1;

    BinarySemaphorePost(jq->has_job);
    pthread_mutex_unlock(&jq->mutex);
}

struct Job *JobQueuePull(struct JobQueue *jq) {
    pthread_mutex_lock(&jq->mutex);
    struct Job *job = NULL;
    if (jq->length == 0) {
        // no job
    } else if (jq->length == 1) {
        job = jq->front;
        jq->front = jq->last = NULL;
        jq->length = 0;
    } else {
        job = jq->front;
        jq->front = job->next;
        --jq->length;

        BinarySemaphorePost(jq->has_job);
    }
    pthread_mutex_unlock(&jq->mutex);

    return job;
}