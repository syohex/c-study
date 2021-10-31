#pragma once

#include <stdbool.h>
#include <pthread.h>

struct BinarySemaphore {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool value;
};

struct BinarySemaphore *BinarySemaphoreCreate();
void BinarySemaphoreDestory(struct BinarySemaphore *bs);
void BinarySemaphoreReset(struct BinarySemaphore *bs);
void BinarySemaphorePost(struct BinarySemaphore *bs);
void BinarySemaphorePostAll(struct BinarySemaphore *bs);
void BinarySemaphoreWait(struct BinarySemaphore *bs);