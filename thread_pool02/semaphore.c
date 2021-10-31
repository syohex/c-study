#include "semaphore.h"

#include <assert.h>
#include <stdlib.h>

static void BinarySemaphoreInit(struct BinarySemaphore *bs) {
    assert(bs != NULL);

    pthread_mutex_init(&bs->mutex, NULL);
    pthread_cond_init(&bs->cond, NULL);
    bs->value = false;
}

struct BinarySemaphore *BinarySemaphoreCreate() {
    struct BinarySemaphore *bs = malloc(sizeof(struct BinarySemaphore));
    if (bs == NULL) {
        return NULL;
    }

    BinarySemaphoreInit(bs);
    return bs;
}

void BinarySemaphoreDestory(struct BinarySemaphore *bs) {
    pthread_mutex_destroy(&bs->mutex);
    pthread_cond_destroy(&bs->cond);
    free(bs);
}

void BinarySemaphoreReset(struct BinarySemaphore *bs) {
    BinarySemaphoreInit(bs);
}

void BinarySemaphorePost(struct BinarySemaphore *bs) {
    pthread_mutex_lock(&bs->mutex);
    bs->value = true;
    pthread_cond_signal(&bs->cond);
    pthread_mutex_unlock(&bs->mutex);
}

void BinarySemaphorePostAll(struct BinarySemaphore *bs) {
    pthread_mutex_lock(&bs->mutex);
    bs->value = true;
    pthread_cond_broadcast(&bs->cond);
    pthread_mutex_unlock(&bs->mutex);
}

void BinarySemaphoreWait(struct BinarySemaphore *bs) {
    pthread_mutex_lock(&bs->mutex);
    while (!bs->value) {
        pthread_cond_wait(&bs->cond, &bs->mutex);
    }
    pthread_mutex_unlock(&bs->mutex);
}