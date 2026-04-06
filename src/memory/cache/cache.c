#include "../../../include/memory/cache/cache.h"
#include "../../../include/memory/RAM/RAM.h"

struct Cache Cache_create(const struct BinSize size, const struct BinSize fetch_size) {
    // Initialize cache to be correct values
    struct Cache cache = { size, fetch_size, 0, 0, NULL };

    // Allocate size in byte form number of uint8s
    struct BinSize bytes = Mem_convert(size, BYTE);

    cache.arr = (uint8_t*)malloc(sizeof(uint8_t) * cache.size.amount);

    return cache;
}

void Cache_destroy(struct Cache* cache) {
    // Free data and set to NULL and prevent use-after-frees
    free(cache->arr);
    cache->arr = NULL;
}

void Cache_fetch(struct Cache* cache, const struct RAM* RAM, const uint16_t index) {
    // Convert fetch size into bytes and half it
    struct BinSize bytes = Mem_convert(cache->fetch_size, BYTE);
    struct BinSize half = { BYTE, bytes.amount / 2 };

    // Get start and end indices
    uint16_t start = index - half.amount;
    uint16_t end = index + half.amount;

    struct BinSize RAM_bytes = Mem_convert(RAM->size, BYTE);

    // Loop over each element
    uint16_t write_i = 0;
    for (uint16_t i = start; i < end; i++) {
        // Bounds check
        if (i < 0 || i > RAM_bytes.amount) {
            cache->arr[write_i] = 0;
            write_i++;
            continue;
        }
        // Copy data into cache
        cache->arr[write_i] = RAM->arr[i];
        write_i++;
    }
}

uint8_t Cache_read(const struct Cache* cache, const uint16_t index) {
    // Return uint8 max if index is out of cache bounds
    if (index < cache->start || index > cache->end) {
        return UINT8_MAX;
    }

    // Otherwise return data in cache
    return cache->arr[index - cache->start];
}