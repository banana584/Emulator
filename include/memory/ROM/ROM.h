#ifndef EMULATOR_MEMORY_ROM_ROM_H
#define EMULATOR_MEMORY_ROM_ROM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "../memutils.h"

/**
 * @struct ROM
 * @brief Representation of Read-Only Memory.
 * @see ROM_create
 * @see ROM_destroy
 * 
 * Represents virtual binary ROM - initialized from a binary file.
 */
struct ROM {
    const char* path; // Path to file on actual disk that data is read from
    const struct BinSize size; // Amount of data stored
    uint8_t* arr; // Const data read from file
};

char* strdup(const char* s);

/**
 * @brief Creates a new ROM instance from a file.
 * @warning ROM_destroy must be called on the result of this function when it is done.
 * @see ROM_destroy
 * 
 * Reads a binary file and converts into a ROM struct instance.
 * 
 * @param path The path of the file to read and create ROM from.
 * @return A new ROM instance with the data found in the file at path.
 */
struct ROM ROM_create(const char* path);

/**
 * @brief Destroys a ROM instance and frees memory.
 * @warning Should only be used when the ROM is no longer needed.
 * 
 * Frees memory in a ROM struct and sets to NULL.
 * 
 * @param ROM The ROM instance to free memory from.
 */
void ROM_destroy(struct ROM* ROM);

/**
 * @brief Reads data at an index from ROM.
 * 
 * This reads a byte of data from ROM at an index.
 * 
 * @param ROM The ROM instance to read from.
 * @param index The index to read data at in ROM.
 * @return The byte of data in ROM.
 */
uint8_t ROM_read(struct ROM* ROM, uint16_t index);

#endif