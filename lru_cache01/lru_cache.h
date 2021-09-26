#pragma once

#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

typedef enum {
    LRU_CACHE_OK,
    LRU_CACHE_KEY_MISSING,
} LRUCacheError;

struct LRUCacheItem {
    char *key;
    void *value;
    size_t value_length;
    uint64_t age;
    void *next;
};

struct LRUCache {
    struct LRUCacheItem **items;
    size_t hash_table_size;
    size_t capacity;
    size_t current_entries;
    uint64_t age;
    time_t seed;
    pthread_mutex_t mutex;
};

// API

struct LRUCache *LRUCacheNew(size_t entries);
void LRUCacheFree(struct LRUCache *cache);
void LRUCacheSet(struct LRUCache *cache, char *key, void *value, size_t value_length);
LRUCacheError LRUCacheGet(struct LRUCache *cache, char *key, void **value, size_t *value_length);
LRUCacheError LRUCacheDelete(struct LRUCache *cache, char *key);
void LRUCacheDump(struct LRUCache *cache);