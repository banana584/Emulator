#ifndef EMULATOR_MEMORY_CACHE_CACHE_H
#define EMULATOR_MEMORY_CACHE_CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include "../memutils.h"

// Forward declaration to prevent circular includes
struct RAM;

/**
 * @struct Cache
 * @brief Represents an emulated cache in a CPU.
 * 
 * Provides a smaller volatile memory than RAM that is closer to the CPU.
 */
struct Cache {
    const struct BinSize size; // Size of data in cache
    const struct BinSize fetch_size; // Amount of data to fetch in total when cache needs to be filled
    uint16_t start; // Start index where data is in RAM
    uint16_t end; // End index where data is in RAM
    uint8_t* arr; // Data currently in cache
};

/**
 * @brief Creates a new instance of a Cache struct.
 * @warning The result of this function must be passed to Cache_destroy when it is no longer needed, otherwise memory will be leaked.
 * @see Cache_destroy
 * 
 * Creates a new Cache struct initialized with data and an empty array.
 * 
 * @param size The size to make the cache.
 * @param fetch_size The amount of data to fetch each time the cache needs to get data from RAM.
 * @return The newly created Cache instance.
 */
struct Cache Cache_create(const struct BinSize size, const struct BinSize fetch_size);

/**
 * @brief Destroys a Cache struct instance.
 * @warning Should only be used when the Cache instance is not needed anymore.
 * 
 * Cleans up the heap data in a Cache struct instance.
 * 
 * @param cache A Cache to delete the data in.
 */
void Cache_destroy(struct Cache* cache);

/**
 * @brief Fetches data around an index from RAM into Cache.
 * 
 * Overrides all data in the cache with data from RAM.
 * 
 * @param cache The Cache to put data into.
 * @param RAM The RAM to read data from.
 * @param index The index to read data around.
 */
void Cache_fetch(struct Cache* cache, const struct RAM* RAM, const uint16_t index);

/**
 * @brief Reads data from a Cache.
 * @warning May return uint8 max if the data is not in the Cache.
 * 
 * Reads data from the Cache, and if it is not in the cache returns uint8 max.
 * 
 * @param cache The Cache to read data from.
 * @param index The index of data in RAM to read from Cache.
 * @return The data in the Cache at index, or uint8 max if it was not found.
 */
uint8_t Cache_read(const struct Cache* cache, const uint16_t index);

#endif