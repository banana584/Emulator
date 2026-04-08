#include "../../../include/memory/RAM/RAM.h"
#include "../../../include/memory/cache/cache.h"

struct RAM RAM_create(const struct BinSize size, struct Cache* cache) {
    // Initialize RAM with values
    struct RAM RAM = { size, NULL, cache, false };

    // Convert size into bytes and allocate that many
    struct BinSize bytes = Mem_convert(size, BYTE);

    RAM.arr = (uint8_t*)malloc(sizeof(uint8_t) * bytes.amount);
    for (uint64_t i = 0; i < bytes.amount; i++) {
        RAM.arr[i] = 0;
    }

    return RAM;
}

void RAM_destroy(struct RAM* RAM) {
    // Prevent use after frees by setting pointer to NULL
    free(RAM->arr);
    RAM->arr = NULL;
}

void RAM_write(struct RAM* RAM, const uint16_t index, const uint8_t data) {
    // Write and mark cache as invalid
    RAM->arr[index] = data;
    if (index > RAM->cache->start && index < RAM->cache->end) {
        RAM->invalidated = true;
    }
}

uint8_t RAM_read(const struct RAM* RAM, const uint16_t index) {
    // Check if cache is invalid or data is not in cache
    uint8_t data = Cache_read(RAM->cache, index);
    if (data == UINT8_MAX || RAM->invalidated) {
        Cache_fetch(RAM->cache, RAM, index);
        return RAM->arr[index];
    }
    return data;
}