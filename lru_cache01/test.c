#include <assert.h>
#include <stdio.h>

#include "lru_cache.h"

int main() {
    {
        struct LRUCache *cache = LRUCacheNew(2);
        uint32_t value1 = 0xdeadbeef;
        LRUCacheSet(cache, "1", &value1, sizeof(uint32_t));

        int value2 = 42;
        LRUCacheSet(cache, "2", &value2, sizeof(int));

        uint32_t *v = NULL;
        size_t v_size;
        LRUCacheError error = LRUCacheGet(cache, "1", (void **)&v, &v_size);
        assert(error == LRU_CACHE_OK);
        assert(*v == 0xdeadbeef);
        assert(v_size == sizeof(uint32_t));

        int value3 = 77;
        LRUCacheSet(cache, "3", &value3, sizeof(int));

        // key=2 is evicted
        int *v2 = NULL;
        error = LRUCacheGet(cache, "2", (void **)&v2, &v_size);
        assert(error == LRU_CACHE_KEY_MISSING);
        assert(v2 == NULL);

        // LRUCacheDump(cache);
        error = LRUCacheDelete(cache, "3");
        assert(error == LRU_CACHE_OK);

        // LRUCacheDump(cache);
        error = LRUCacheDelete(cache, "3");
        assert(error == LRU_CACHE_KEY_MISSING);

        LRUCacheFree(cache);
    }

    printf("OK\n");
    return 0;
}