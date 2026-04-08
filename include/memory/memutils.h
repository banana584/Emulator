#ifndef EMULATOR_MEMORY_MEMORY_H
#define EMULATOR_MEMORY_MEMORY_H

#include <stdio.h>
#include <stdint.h>
#include <math.h>

/**
 * @enum DataSize
 * @brief An enum of different byte sizes.
 * 
 * Represents different byte sizes needed in this project.
 */
typedef enum DataSize {
    BYTE, // Single byte
    KILOBYTE, // 1024 bytes
    MEGABYTE, // 1024 kilobytes
    GIGABYTE // 1024 megabytes
} DataSize;

/**
 * @struct BinSize
 * @brief Represents a binary size
 * @see DataSize
 * @see Mem_convert
 * @see Mem_print
 * 
 * A binary size, whether that be a single byte or up to a gigabyte. Can be converted to any other supported size in the DataSize enum.
 */
struct BinSize {
    DataSize unit; // The units amount is measured in.
    double amount; // The amount of data measured in units.
};

/**
 * @brief Converts one BinSize into another unit.
 * 
 * Takes in a BinSize and converts it into different units.
 * 
 * @param current A BinSize to be converted into different units. Is not modified.
 * @param target The new units to convert current into.
 * @return The new BinSize with requested units.
 */
struct BinSize Mem_convert(const struct BinSize current, const DataSize target);

/**
 * @brief Simplifies a BinSize into its smallest form.
 * 
 * Converts a BinSize downwards if it can, and upwards if it is too small.
 * 
 * @param current A BinSize to be simplified. Is not modified.
 * @return The new simplified BinSize instance.
 */
struct BinSize Mem_simplify(const struct BinSize current);

/**
 * @brief Prints a BinSize instance to stdout.
 * 
 * Prints an instance of the BinSize struct. Uses the format of "{amount} {unit}" where unit is the shorthand form.
 * 
 * @param size The BinSize to print.
 */
void Mem_print(const struct BinSize size);

#endif