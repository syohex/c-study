#include "thread.h"

#include <stdlib.h>

struct Thread *ThreadCreate(struct ThreadPool *pool, int id, void *(*func)(void *)) {
    struct Thread *t = malloc(sizeof(struct Thread));
    if (t == NULL) {
        return NULL;
    }

    t->id = id;
    t->pool = pool;

    pthread_create(&t->thread, NULL, func, t);
    pthread_detach(t->thread);
    return t;
}

void ThreadDestory(struct Thread *t) {
    free(t);
}