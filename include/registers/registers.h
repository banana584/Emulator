#ifndef EMULATOR_REGISTERS_REGISTERS_H
#define EMULATOR_REGISTERS_REGISTERS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// Helper bit check macro
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

/**
 * @enum RegisterType
 * @brief Represents the type of register.
 * 
 * Represents each type of register the CPU can have. Based off of 6502 registers.
 */
typedef enum RegisterType {
    PROGRAM_COUNTER1, // Left side of pc
    PROGRAM_COUNTER2, // Right side of pc
    ACCUMULATOR_A, // Accumulator - stores numbers for operations and stores result of operations
    ACCUMULATOR_B, // Accumulator B - stores numbers for operations
    X_REG, // Side register that can be swapped with accumulator
    Y_REG, // Side register than can be swapped with accumulator
    STACK_POINTER, // Stores address of the top of the stack
    STATUS, // The status of processor and last operation
    NUM_REG_TYPES // Number of registers
} RegisterType;

/**
 * @struct Register
 * @brief Represents a register
 * 
 * Stores an 8 bit value close the the CPU.
 */
struct Register {
    RegisterType type; // The type of register, should be unique.
    uint8_t data; // The data stored in the register.
};

/**
 * @struct RegisterArray
 * @brief Represents all the registers in a CPU.
 * @see Register
 * @see Register_create
 * @see Register_destroy
 * 
 * Stores a heap array of registers that can be written to or read from.
 */
struct RegisterArray {
    uint8_t size;
    struct Register* arr;
};

/**
 * @struct Status
 * @brief Represents packed data in status register.
 * 
 * Stores 7 boolean values, representing the state of the processor and last operation.
 */
struct Status {
    bool carry; // Set if operation caused overflow in bit 7 or underflow in bit 0
    bool zero; // Set if last operation was 0
    bool int_disable; // Set if interrupts are disabled
    bool decimal_mode; // Set if processor will follow BCD rules
    bool break_command; // Set when BRK is called and an interrupt is generated for it
    bool overflow; // Set if a result is invalid, meaning there was an overflow
    bool negative; // Set if the result was negative in 2s complement
};

/**
 * @brief Creates a new instance of the RegisterArray struct.
 * @warning Register_destroy must be used on the result when it is done, otherwise memory will be leaked.
 * @see RegisterArray
 * @see Register_destroy
 * 
 * Creates a new RegisterArray on the stack, and allocates the arr member on the heap.
 */
struct RegisterArray Register_create();

/**
 * @brief Destroyes the data in a RegisterArray struct.
 * @warning Should only be used when the register array is not needed.
 * 
 * Destroyes heap data of the RegisterArray struct and assignes NULL to it.
 */
void Register_destroy(struct RegisterArray* registers);

/**
 * @brief Converts bytes in a uint8 to a Status struct.
 * @see Status
 * 
 * Reads bytes from a uint8 and converts them into a Status struct.
 * 
 * @param data A uint8 to convert into a Status struct.
 * @return A Status struct constructed from the bytes in data.
 */
struct Status Register_bytes_to_status(const uint8_t data);

/**
 * @brief Converts a Status struct into bytes.
 * @see Status
 * 
 * Reads the data in a Status struct and converts it into bytes.
 * 
 * @param status The Status struct to read and convert to bytes.
 * @return The bytes converted from the Status struct.
 */
uint8_t Register_status_to_bytes(const struct Status status);

/**
 * @brief Writes an 8 bit value into a register.
 * @warning Should not be used on program counter.
 * @warning Using on status may lead to bit shifting errors.
 * 
 * Writes data into a registers array at type index.
 * 
 * @param registers The array of registers to write to.
 * @param type The register to write to.
 * @param data The 8 bit value to write into the register.
 */
void Register_write(struct RegisterArray* registers, const RegisterType type, const uint8_t data);

/**
 * @brief Reads an 8 bit value from a register.
 * @warning Should not be used on program counter.
 * @warning Using on status may lead to bit shifting errors.
 * 
 * Reads data from a registers array at type index.
 * 
 * @param registers The array of registers to read from.
 * @param type The type of register to read from.
 * @return The 8 bit value read from the register.
 */
uint8_t Register_read(const struct RegisterArray* registers, const RegisterType type);

/**
 * @brief Writes a program counter to registers.
 * 
 * Splits 16 bit value into 2 8 bit values and writes to registers.
 * 
 * @param registers The register array to write to.
 * @param data The 16 bit program counter to write.
 */
void Register_write_pc(struct RegisterArray* registers, const uint16_t data);

/**
 * @brief Reads a program counter from registers.
 * 
 * Reads 2 8 bit values from registers and merges into 1 16 bit value.
 * 
 * @param registers The register array to read from.
 * @return The current program counter value.
 */
uint16_t Register_read_pc(const struct RegisterArray* registers);

/**
 * @brief Writes a Status struct to registers.
 * 
 * Converts a Status struct into bytes and writes it into registers.
 * 
 * @param registers The register array to write to.
 * @param data The Status struct to write to registers.
 */
void Register_write_status(struct RegisterArray* registers, const struct Status data);

/**
 * @brief Reads a Status struct from registers.
 * 
 * Reads bytes from registers and converts it into a Status struct.
 * 
 * @param registers The register array to read from.
 * @return The Status struct read from registers.
 */
struct Status Register_read_status(const struct RegisterArray* registers);

#endif