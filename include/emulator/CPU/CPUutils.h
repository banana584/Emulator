#ifndef EMULATOR_CPU_CPUUTILS_H
#define EMULATOR_CPU_CPUUTILS_H

#include <stdint.h>

// Forward declaration
struct CPU;

// Typedef of Instruction function pointer
typedef void (*Instruction)(struct CPU*, uint16_t);

#endif