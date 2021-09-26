#include "lru_cache.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// MurmurHash2, by Austin Appleby
// http://sites.google.com/site/murmurhash/
static size_t LRUCacheGetHashIndex(struct LRUCache *cache, char *key) {
    size_t key_len = strlen(key);
    uint32_t m = 0x5bd1e995;
    uint32_t r = 24;
    uint32_t h = cache->seed ^ key_len;
    char *data = (char *)key;

    while (key_len >= 4) {
        uint32_t k = *(uint32_t *)data;
        k *= m;
        k ^= k >> r;
        k *= m;
        h *= m;
        h ^= k;
        data += 4;
        key_len -= 4;
    }

    switch (key_len) {
    case 3:
        h ^= data[2] << 16;
        // fallthrough
    case 2:
        h ^= data[1] << 8;
        // fallthrough
    case 1:
        h ^= data[0];
        h *= m;
        break;
    default:
        break;
    }

    h ^= h >> 13;
    h *= m;
    h ^= h >> 16;
    return h % cache->hash_table_size;
}

// Return 0 if key is found, otherwise return not 0
static int LRUCacheCompareKey(struct LRUCacheItem *item, char *key) {
    size_t key_len = strlen(key);
    if (key_len != strlen(item->key)) {
        return 1;
    }

    return memcmp(key, item->key, key_len);
}

static void LRUCacheRemoveItem(struct LRUCache *cache, struct LRUCacheItem *prev, struct LRUCacheItem *item, size_t index) {
    if (prev != NULL) {
        prev->next = item->next;
    } else {
        cache->items[index] = (struct LRUCacheItem *)item->next;
    }

    free(item->value);
    free(item->key);
}

static void LRUCacheRemoveLeastUsedItem(struct LRUCache *cache) {
    struct LRUCacheItem *least_used = NULL;
    struct LRUCacheItem *least_prev = NULL;
    size_t least_index = 0;
    int least_age = INT_MAX;

    for (size_t i = 0; i < cache->hash_table_size; ++i) {
        struct LRUCacheItem *item = cache->items[i];
        struct LRUCacheItem *prev = NULL;
        while (item != NULL) {
            if (item->age < least_age) {
                least_age = item->age;
                least_used = item;
                least_prev = prev;
                least_index = i;
            }

            prev = item;
            item = (struct LRUCacheItem *)item->next;
        }
    }

    if (least_used != NULL) {
        LRUCacheRemoveItem(cache, least_prev, least_used, least_index);
        --cache->current_entries;
    }
}

struct LRUCache *LRUCacheNew(size_t entries) {
    struct LRUCache *cache = (struct LRUCache *)malloc(sizeof(struct LRUCache));
    if (cache == NULL) {
        perror("malloc");
        return NULL;
    }

    cache->capacity = entries;
    cache->hash_table_size = entries;
    cache->current_entries = 0;
    cache->seed = time(NULL);

    cache->items = (struct LRUCacheItem **)malloc(sizeof(struct LRUCacheItem *) * cache->hash_table_size);
    if (cache->items == NULL) {
        perror("malloc");
        free(cache);
        return NULL;
    }

    pthread_mutex_init(&cache->mutex, NULL);
    return cache;
}

void LRUCacheFree(struct LRUCache *cache) {
    if (cache->items != NULL) {
        for (size_t i = 0; i < cache->hash_table_size; ++i) {
            struct LRUCacheItem *item = cache->items[i];
            while (item != NULL) {
                struct LRUCacheItem *next = (struct LRUCacheItem *)item->next;
                free(item->key);
                free(item->value);
                free(item);
                item = next;
            }
        }

        free(cache->items);
    }

    pthread_mutex_destroy(&cache->mutex);
    free(cache);
}

void LRUCacheSet(struct LRUCache *cache, char *key, void *value, size_t value_length) {
    pthread_mutex_lock(&cache->mutex);

    size_t hash_index = LRUCacheGetHashIndex(cache, key);
    struct LRUCacheItem *item = cache->items[hash_index];
    struct LRUCacheItem *prev = NULL;

    while (item != NULL && LRUCacheCompareKey(item, key) != 0) {
        prev = item;
        item = (struct LRUCacheItem *)item->next;
    }

    if (item != NULL) {
        free(item->value);

        item->value = malloc(value_length);
        memcpy(item->value, value, value_length);
        item->value_length = value_length;
    } else {
        if (cache->current_entries >= cache->capacity) {
            LRUCacheRemoveLeastUsedItem(cache);
        }

        // insert a new item
        size_t key_len = strlen(key);
        item = (struct LRUCacheItem *)malloc(sizeof(struct LRUCacheItem));
        item->key = malloc(key_len + 1);
        strcpy(item->key, key);
        item->key[key_len] = '\0';

        item->value = malloc(value_length);
        memcpy(item->value, value, value_length);
        item->value_length = value_length;

        if (prev != NULL) {
            prev->next = item;
        } else {
            cache->items[hash_index] = item;
        }

        ++cache->current_entries;
    }

    item->age = cache->age;
    ++cache->age;

    pthread_mutex_unlock(&cache->mutex);
}

LRUCacheError LRUCacheGet(struct LRUCache *cache, char *key, void **value, size_t *value_length) {
    LRUCacheError ret = LRU_CACHE_OK;
    pthread_mutex_lock(&cache->mutex);

    size_t hash_index = LRUCacheGetHashIndex(cache, key);
    struct LRUCacheItem *item = cache->items[hash_index];
    while (item != NULL && LRUCacheCompareKey(item, key)) {
        item = (struct LRUCacheItem *)item->next;
    }

    if (item != NULL) {
        *value = item->value;
        *value_length = item->value_length;
        item->age = cache->age;
        ++cache->age;
    } else {
        *value = NULL;
        *value_length = 0;
        ret = LRU_CACHE_KEY_MISSING;
    }

    pthread_mutex_unlock(&cache->mutex);
    return ret;
}

LRUCacheError LRUCacheDelete(struct LRUCache *cache, char *key) {
    LRUCacheError ret = LRU_CACHE_OK;
    pthread_mutex_lock(&cache->mutex);

    size_t hash_index = LRUCacheGetHashIndex(cache, key);
    struct LRUCacheItem *item = cache->items[hash_index];
    struct LRUCacheItem *prev = NULL;
    while (item != NULL && LRUCacheCompareKey(item, key)) {
        prev = item;
        item = (struct LRUCacheItem *)item->next;
    }

    if (item != NULL) {
        LRUCacheRemoveItem(cache, prev, item, hash_index);
    } else {
        ret = LRU_CACHE_KEY_MISSING;
    }

    pthread_mutex_unlock(&cache->mutex);
    return ret;
}

void LRUCacheDump(struct LRUCache *cache) {
    size_t num = 0;
    for (size_t i = 0; i < cache->hash_table_size; ++i) {
        struct LRUCacheItem *item = cache->items[i];
        while (item != NULL) {
            printf("item%zd key=%s age=%lu\n", num++, item->key, item->age);
            item = (struct LRUCacheItem *)item->next;
        }
    }
}