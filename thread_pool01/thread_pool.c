#include "thread_pool.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

static struct WorkerTask *GetWorkerTask(struct ThreadPool *tp) {
    struct WorkerTask *p = tp->task_list;
    if (p == NULL) {
        return NULL;
    }

    tp->task_list = p->next;
    return p;
}

static void *Worker(void *arg) {
    struct ThreadPool *tp = arg;

    while (tp->running) {
        pthread_mutex_lock(&tp->mutex);

        while (tp->running && tp->task_list == NULL) {
            pthread_cond_wait(&tp->task_list_cond, &tp->mutex);
        }

        if (!tp->running) {
            break;
        }

        struct WorkerTask *task = GetWorkerTask(tp);
        ++tp->working_count;
        pthread_mutex_unlock(&tp->mutex);

        if (task != NULL) {
            task->func(task->arg);
            free(task);
        }

        pthread_mutex_lock(&tp->mutex);
        --tp->working_count;
        if (tp->running && tp->working_count == 0 && tp->task_list == NULL) {
            pthread_cond_signal(&tp->finish_cond);
        }
        pthread_mutex_unlock(&tp->mutex);
    }

    --tp->thread_count;
    pthread_cond_signal(&tp->finish_cond);
    pthread_mutex_unlock(&tp->mutex);
    return NULL;
}

struct ThreadPool *ThreadPoolCreate(size_t num) {
    if (num == 0) {
        errno = EINVAL;
        return NULL;
    }

    struct ThreadPool *tp = malloc(sizeof(struct ThreadPool));
    if (tp == NULL) {
        return NULL;
    }

    pthread_mutex_init(&tp->mutex, NULL);
    pthread_cond_init(&tp->task_list_cond, NULL);
    pthread_cond_init(&tp->finish_cond, NULL);

    tp->task_list = NULL;
    tp->working_count = 0;
    tp->thread_count = num;
    tp->running = true;

    for (size_t i = 0; i < num; ++i) {
        pthread_t thread;

        pthread_create(&thread, NULL, Worker, tp);
        pthread_detach(thread);
    }

    return tp;
}

void ThreadPoolDestory(struct ThreadPool *tp) {
    pthread_mutex_lock(&tp->mutex);

    struct WorkerTask *p = tp->task_list;
    while (p != NULL) {
        struct WorkerTask *next = p->next;
        free(p);
        p = next;
    }
    tp->running = false;
    pthread_cond_broadcast(&tp->task_list_cond);
    pthread_mutex_unlock(&tp->mutex);

    ThreadPoolWait(tp);

    pthread_mutex_destroy(&tp->mutex);
    pthread_cond_destroy(&tp->task_list_cond);
    pthread_cond_destroy(&tp->finish_cond);

    free(tp);
}

bool ThreadPoolAddTask(struct ThreadPool *tp, ThreadFunc func, void *arg) {
    struct WorkerTask *task = malloc(sizeof(struct WorkerTask));
    if (task == NULL) {
        return false;
    }

    task->func = func;
    task->arg = arg;
    task->next = NULL;

    pthread_mutex_lock(&tp->mutex);
    struct WorkerTask *p = tp->task_list;
    if (p == NULL) {
        tp->task_list = task;
    } else {
        struct WorkerTask *prev = NULL;
        while (p != NULL) {
            prev = p;
            p = p->next;
        }
        prev->next = task;
    }

    pthread_cond_signal(&tp->task_list_cond);
    pthread_mutex_unlock(&tp->mutex);
    return true;
}

void ThreadPoolWait(struct ThreadPool *tp) {
    pthread_mutex_lock(&tp->mutex);
    while (true) {
        if ((tp->running && tp->working_count > 0) || (!tp->running && tp->thread_count > 0)) {
            pthread_cond_wait(&tp->finish_cond, &tp->mutex);
        } else {
            break;
        }
    }
    pthread_mutex_unlock(&tp->mutex);
}
