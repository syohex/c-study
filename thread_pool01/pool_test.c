#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "thread_pool.h"

static pthread_mutex_t mutex;
static int count = 0;

static void Task(void *arg) {
    pthread_mutex_lock(&mutex);
    ++count;
    pthread_mutex_unlock(&mutex);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s thread_num\n", argv[0]);
        return -1;
    }

    pthread_mutex_init(&mutex, NULL);

    int threads = atoi(argv[1]);
    struct ThreadPool *tp = ThreadPoolCreate(threads);
    if (tp == NULL) {
        perror("ThreadPoolCreate");
        return -1;
    }

    int tasks = threads * 10000;
    for (int i = 0; i < tasks; ++i) {
        ThreadPoolAddTask(tp, Task, NULL);
    }

    ThreadPoolWait(tp);
    ThreadPoolDestory(tp);
    printf("## count=%d\n", count);
    return 0;
}