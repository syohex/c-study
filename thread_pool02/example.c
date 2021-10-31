#include <stdio.h>
#include <stdint.h>

#include "thread_pool.h"

static void Task(void *arg) {
    printf("Thread %u working on %d\n", (unsigned int)pthread_self(), (int)((intptr_t)arg));
}

int main() {
    struct ThreadPool *pool = ThreadPoolCreate(8);
    if (pool == NULL) {
        printf("failed to create thread pool\n");
        return 1;
    }

    for (int i = 0; i < 80; ++i) {
        ThreadPoolAddTask(pool, Task, (void *)((intptr_t)i));
    }

    ThreadPoolWait(pool);
    ThreadPoolDestory(pool);
    printf("## Finish\n");
    return 0;
}