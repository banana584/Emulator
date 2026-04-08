#ifndef EMULATOR_MEMORY_RAM_RAM_H
#define EMULATOR_MEMORY_RAM_RAM_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../memutils.h"

// Forward declaration to prevent circular includes
struct Cache;

/**
 * @struct RAM
 * @brief Represents RAM in an emulated computer.
 * 
 * Contains a heap array of arbitrary size to represent RAM. Additionally has a cache to improve speed.
 */
struct RAM {
    const struct BinSize size; // The size of RAM
    uint8_t* arr; // Heap array of data
    struct Cache* cache; // Cache to improve speed
    bool invalidated; // If the cache has been invalidated
};

/**
 * @brief Creates a new instance of RAM struct.
 * @warning RAM_destroy must be used on the return of this function, otherwise memory will be leaked.
 * @see RAM_destroy
 * 
 * Makes a new RAM struct instance on the stack and allocates the array on the heap.
 * 
 * @param size The size of RAM to make.
 * @param cache A pointer to an existing cache to get a pointer to in the new RAM.
 * @return The instance of the RAM struct that was made.
 */
struct RAM RAM_create(const struct BinSize size, struct Cache* cache);

/**
 * @brief Destroys a RAM struct instance.
 * @warning Should only be used when RAM is not needed anymore.
 * @warning Cache is not destroyed.
 * 
 * Frees heap memory in a RAM struct instance and assigns pointer to NULL.
 * 
 * @param RAM A pointer to a RAM struct instance to destroy.
 */
void RAM_destroy(struct RAM* RAM);

/**
 * @brief Writes data at an address to RAM.
 * 
 * Writes a byte of data into RAM at an index, and marks the cache as invalidated.
 * 
 * @param RAM The RAM instance to write to.
 * @param index The index to write data at.
 * @param data The data to write into RAM.
 */
void RAM_write(struct RAM* RAM, const uint16_t index, const uint8_t data);

/**
 * @brief Reads data at an address from RAM.
 * 
 * Checks if the requested index is in cache, and if it is not or the cache is marked as invalidated the cache is fetched and data is read.
 * 
 * @param RAM The RAM instance to read from.
 * @param index The index to read data from.
 * @return The data stored in RAM at index.
 */
uint8_t RAM_read(const struct RAM* RAM, const uint16_t index);

#endif