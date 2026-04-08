#ifndef EMULATOR_CPU_CPU_H
#define EMULATOR_CPU_CPU_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include "../registers/registers.h"
#include "../memory/RAM/RAM.h"
#include "../memory/cache/cache.h"
#include "../memory/ROM/ROM.h"
#include "CPUutils.h"

// Map of every supported instruction
extern Instruction instruction_map[256];

// Forward declarations
struct Interrupt;
struct InterruptQueue;

/**
 * @enum CycleAmount
 * @brief Represents units of CPU cycles.
 * 
 * Enum of different units of CPU clock speed.
 */
typedef enum CycleAmount {
    HERTZ, // Hertz - the lowest unit
    KILOHERTZ, // Kilohertz - 1000 hertz
    MEGAHERTZ, // Megahertz - 1000 kilohertz
    GIGAHERTZ // Gigahertz - 1000 megahertz
} CycleAmount;

/**
 * @struct ClockSpeed
 * @brief Represents a certain amount of cycles in a unit.
 * 
 * Clock speed measured in any unit in the CycleAmount enum.
 */
struct ClockSpeed {
    CycleAmount unit; // The units amount is stored in
    double amount; // How many of the unit
};

/**
 * @brief Converts a ClockSpeed struct into a different unit.
 * 
 * Converts a ClockSpeed into different units by multiplying or diving by 1000.
 * 
 * @param current The current ClockSpeed to convert. Is not modified.
 * @param target The unit to convert ClockSpeed into.
 * @return The new converted ClockSpeed.
 */
struct ClockSpeed CPU_convert(const struct ClockSpeed current, CycleAmount target);

/**
 * @brief Prints a CPU ClockSpeed.
 * 
 * Prints the amount variable, then the shortened unit.
 * 
 * @param speed The ClockSpeed to print.
 */
void CPU_print_clockspeed(const struct ClockSpeed speed);

/**
 * @struct Clock
 * @brief A CPU Clock to limit the speed of the CPU.
 * 
 * Represents the Clock in a CPU to follow a specified speed.
 */
struct Clock {
    struct ClockSpeed target; // Target speed to reach / limit to
    time_t start; // Time clock was started
};

/**
 * @brief Creates a new Clock instance on the stack.
 * 
 * Creates an empty clock, with the requested target speed. Does not start the clock.
 * 
 * @param target The speed for the clock to try to reach.
 * @return The new Clock instance.
 */
struct Clock Clock_create(const struct ClockSpeed target);

/**
 * @brief Starts a Clock's timer.
 * 
 * Sets the start time of a Clock to the current time.
 * 
 * @param clock The clock to start.
 */
void Clock_start(struct Clock* clock);

/**
 * @brief Ends a Clock's timer and waits until the target is reached.
 * @warning Blocks the current thread.
 * 
 * Ends the time of a Clock, then calculates the time to wait and sleeps for that time.
 * 
 * @param clock The clock to end.
 */
void Clock_end(struct Clock* clock);

/**
 * @struct CPU
 * @brief A representation of a virtual processor
 * @warning CPU is not responsible for the member variables. They must be cleaned up elsewhere.
 * 
 * Represents the processor in an emulated computer.
 */
struct CPU {
    struct RAM* RAM; // RAM used in CPU
    struct RegisterArray* registers; // Registers used in CPU
    struct ROM* ROM; // Program ROM to run in CPU
    struct Clock* clock; // Clock to control timings of CPU
    struct InterruptQueue* interrupts; // Interrupt queue to check at end of cycle in CPU
    bool on; // If the CPU should continue to run.
};

/**
 * @brief Creates a new instance of the CPU struct.
 * 
 * Creates a new stack instance of the CPU struct with data inside it.
 * 
 * @param RAM The RAM to be used in CPU cycles.
 * @param registers The registers to be used for CPU fast memory.
 * @param ROM The program ROM to be ran.
 * @param clock The Clock constrolling the CPU speed.
 * @param interrupts The Interrupt Queue to be checked after each cycle.
 * @return The new stack CPU instance.
 */
struct CPU CPU_create(struct RAM* RAM, struct RegisterArray* registers, struct ROM* ROM, struct Clock* clock, struct InterruptQueue* interrupts);

/**
 * @brief Runs a single CPU cycle.
 * 
 * Runs a singular cycle on a CPU from program ROM, then checks the Interrupt queue.
 * 
 * @param CPU The CPU to run a cycle on.
 */
void CPU_run_cycle(struct CPU* CPU);

/**
 * @brief Runs a specified amount of cycles on a CPU.
 * @warning Blocks the current thread until it is done.
 * 
 * Runs a specified amount of cycles on a CPU from program ROM, checking the Interrupt queue after each cycle.
 * 
 * @param CPU The CPU to run the cycles on.
 * @param max_cycles The amount of cycles to run on the CPU.
 */
void CPU_run(struct CPU* CPU, const uint64_t max_cycles);

/**
 * @brief Runs unlimited cycles on the CPU.
 * @warning Blocks the current thread unless the program finished, the CPU is stopped forcefully or via Interrupts.
 * 
 * Runs unlimited cycles on the CPU, until the program is finished or the CPU is stopped.
 * 
 * @param CPU The CPU to run the program on.
 */
void CPU_run_limitless(struct CPU* CPU);

// ===== Instructions =====

/**
 * @brief Loads a value into Accumulator A.
 * 
 * Loads the value in param into the CPU register A.
 * 
 * @param CPU The CPU whose registers should be modified.
 * @param param The value to load into Accumulator A.
 */
void CPU_LDA(struct CPU* CPU, uint16_t param);

/**
 * @brief Loads a value in RAM into Accumulator A.
 * 
 * Loads a value from RAM at param into the CPU register A.
 * 
 * @param CPU The CPU whose registers should be modified.
 * @param param The address of the value to load into Accumulator A.
 */
void CPU_LDA_addr(struct CPU* CPU, uint16_t param);

/**
 * @brief Loads a value into Accumulator B.
 * 
 * Loads the value in param into the CPU register B.
 * 
 * @param CPU The CPU whose registers should be modified.
 * @param param The value to load into Accumulator B.
 */
void CPU_LDB(struct CPU* CPU, uint16_t param);

/**
 * @brief Loads a value in RAM into Accumulator B.
 * 
 * Loads a value from RAM at param into the CPU register B.
 * 
 * @param CPU The CPU whose registers should be modified.
 * @param param The address of the value to load into Accumulator B.
 */
void CPU_LDB_addr(struct CPU* CPU, uint16_t param);

/**
 * @brief Loads a value into Index Register X.
 * 
 * Loads the value in param into the CPU register X.
 * 
 * @param CPU The CPU whose registers should be modified.
 * @param param The value to load into Index Register X.
 */
void CPU_LDX(struct CPU* CPU, uint16_t param);

/**
 * @brief Loads a value in RAM into Index Register X.
 * 
 * Loads a value from RAM at param into the CPU register X.
 * 
 * @param CPU The CPU whose registers should be modified.
 * @param param The address of the value to load into Index Register X.
 */
void CPU_LDX_addr(struct CPU* CPU, uint16_t param);

/**
 * @brief Loads a value into Index Register Y.
 * 
 * Loads the value in param into the CPU register Y.
 * 
 * @param CPU The CPU whose registers should be modified.
 * @param param THe value to load into Index Register Y.
 */
void CPU_LDY(struct CPU* CPU, uint16_t param);

/**
 * @brief Loads a value in RAM into Index Register Y.
 * 
 * Loads a value from RAM at param into the CPU register Y.
 * 
 * @param CPU The CPU whose registers should be modified.
 * @param param The address of the value to load into Index Register Y.
 */
void CPU_LDY_addr(struct CPU* CPU, uint16_t param);

/**
 * @brief Stores the value in Accumulator A onto the stack.
 * 
 * Stores the value in Accumulator A into the stack.
 * 
 * @param CPU The CPU whose registers should be read.
 * @param param Unused.
 */
void CPU_STA(struct CPU* CPU, uint16_t param);

/**
 * @brief Stores the value in Accumulator B onto the stack.
 * 
 * Stores the value in Accumulator B into the stack.
 * 
 * @param CPU The CPU whose registers should be read.
 * @param param Unused.
 */
void CPU_STB(struct CPU* CPU, uint16_t param);

/**
 * @brief Stores the value in Index Register X onto the stack.
 * 
 * Stores the value in Index Register X into the stack.
 * 
 * @param CPU The CPU whose registers should be read.
 * @param param Unused.
 */
void CPU_STX(struct CPU* CPU, uint16_t param);

/**
 * @brief Stores the value in Index Register Y onto the stack.
 * 
 * Stores the value in Index Register Y into the stack.
 * 
 * @param CPU The CPU whose registers should be read.
 * @param param Unused.
 */
void CPU_STY(struct CPU* CPU, uint16_t param);

/**
 * @brief Initiates a software interrupt.
 * 
 * Triggers the Break flag in the CPU.
 * 
 * @param CPU The CPU whose break flag should be triggered.
 * @param param Unused.
 */
void CPU_BRK(struct CPU* CPU, uint16_t param);

/**
 * @brief Compares a value against Accumulator A.
 * 
 * Compares param against register A and sets status flags accordingly.
 * 
 * @param CPU The CPU whose registers should be read and set.
 * @param param The value to compare against A.
 */
void CPU_CPA(struct CPU* CPU, uint16_t param);

/**
 * @brief Compares a value in RMA against Accumulator A.
 * 
 * Compares the value in RAM at param against register A and sets status flags accordingly.
 * 
 * @param CPU The CPU whose registers should be read and set.
 * @param param The address of the value to compare against A.
 */
void CPU_CPA_addr(struct CPU* CPU, uint16_t param);

/**
 * @brief Compares a value against Accumulator B.
 * 
 * Compares param against register B and sets status flags accordingly.
 * 
 * @param CPU The CPU whose registers should be read and set.
 * @param param The value to compare against B.
 */
void CPU_CPB(struct CPU* CPU, uint16_t param);

/**
 * @brief Compares a value in RMA against Accumulator B.
 * 
 * Compares the value in RAM at param against register B and sets status flags accordingly.
 * 
 * @param CPU The CPU whose registers should be read and set.
 * @param param The address of the value to compare against B.
 */
void CPU_CPB_addr(struct CPU* CPU, uint16_t param);

/**
 * @brief Compares a value against Index Register X.
 * 
 * Compares param against register X and sets status flags accordingly.
 * 
 * @param CPU The CPU whose registers should be read and set.
 * @param param The value to compare against X.
 */
void CPU_CPX(struct CPU* CPU, uint16_t param);

/**
 * @brief Compares a value in RMA against Index Register X.
 * 
 * Compares the value in RAM at param against register X and sets status flags accordingly.
 * 
 * @param CPU The CPU whose registers should be read and set.
 * @param param The address of the value to compare against X.
 */
void CPU_CPX_addr(struct CPU* CPU, uint16_t param);

/**
 * @brief Compares a value against Index Register Y.
 * 
 * Compares param against register Y and sets status flags accordingly.
 * 
 * @param CPU The CPU whose registers should be read and set.
 * @param param The value to compare against Y.
 */
void CPU_CPY(struct CPU* CPU, uint16_t param);

/**
 * @brief Compares a value in RMA against Index Register Y.
 * 
 * Compares the value in RAM at param against register Y and sets status flags accordingly.
 * 
 * @param CPU The CPU whose registers should be read and set.
 * @param param The address of the value to compare against Y.
 */
void CPU_CPY_addr(struct CPU* CPU, uint16_t param);

/**
 * @brief Adds Accumulator A and B.
 * 
 * Adds the values in Accumulators A and B, stores in A and sets status flags.
 * 
 * @param CPU The CPU to do the operation.
 * @param param Unused.
 */
void CPU_ADD(struct CPU* CPU, uint16_t param);

/**
 * @brief Subtracts Accumulator A and B.
 * 
 * Subtracts the values in Accumulators A and B, stores in A and sets status flags.
 * 
 * @param CPU The CPU to do the operation.
 * @param param Unused.
 */
void CPU_SUB(struct CPU* CPU, uint16_t param);

/**
 * @brief Multiplies Accumulator A and B.
 * 
 * Multiplies the values in Accumulators A and B, stores in A and sets status flags.
 * 
 * @param CPU The CPU to do the operation.
 * @param param Unused.
 */
void CPU_MUL(struct CPU* CPU, uint16_t param);

/**
 * @brief Divides Accumulator A and B.
 * 
 * Divides the values in Accumulators A and B, stores in A and sets status flags.
 * 
 * @param CPU The CPU to do the operation.
 * @param param Unused.
 */
void CPU_DIV(struct CPU* CPU, uint16_t param);

/**
 * @brief Increments a value in memory.
 * 
 * Reads a value in RAM at an address, increments it and writes it back to the same address.
 * 
 * @param CPU The CPU to do the operation.
 * @param param The address of the value in RAM to increment.
 */
void CPU_INC(struct CPU* CPU, uint16_t param);

/**
 * @brief Decrements a value in memory.
 * 
 * Reads a value in RAM at an address, decrements it and writes it back to the same address.
 * 
 * @param CPU The CPU to do the operation.
 * @param param The address of the value in RAM to increment.
 */
void CPU_DEC(struct CPU* CPU, uint16_t param);

/**
 * @brief Push the current status onto the stack.
 * 
 * Gets the status of the CPU, then pushes it onto the stack.
 * 
 * @param CPU The CPU whose status should be read.
 * @param param Unused.
 */
void CPU_PPS(struct CPU* CPU, uint16_t param);

/**
 * @brief Pull the last status off the stack.
 * 
 * Gets the status from RAM, then sets the CPU to that.
 * 
 * @param CPU The CPU whose status should be set.
 * @param param Unused.
 */
void CPU_RPS(struct CPU* CPU, uint16_t param);

/**
 * @brief Transfer the value from Accumulator A into Index Register X.
 * 
 * Reads Accumulator A and sets the value into Index Register X.
 * 
 * @param CPU The CPU whose registers should be read and set.
 * @param param Unused.
 */
void CPU_TAX(struct CPU* CPU, uint16_t param);

/**
 * @brief Transfer the value from Accumulator B into Index Register X.
 * 
 * Reads Accumulator B and sets the value into Index Register X.
 * 
 * @param CPU The CPU whose registers should be read and set.
 * @param param Unused.
 */
void CPU_TBX(struct CPU* CPU, uint16_t param);

/**
 * @brief Transfer the value from Index Register Y into Index Register X.
 * 
 * Reads Index Register Y and sets the value into Index Register X.
 * 
 * @param CPU The CPU whose registers should be read and set.
 * @param param Unused.
 */
void CPU_TYX(struct CPU* CPU, uint16_t param);

/**
 * @brief Transfer the value from Accumulator A into Index Register Y.
 * 
 * Reads Accumulator A and sets the value into Index Register Y.
 * 
 * @param CPU The CPU whose registers should be read and set.
 * @param param Unused.
 */
void CPU_TAY(struct CPU* CPU, uint16_t param);

/**
 * @brief Transfer the value from Accumulator B into Index Register Y.
 * 
 * Reads Acumulator B and sets the value into Index Register Y.
 * 
 * @param CPU The CPU whose registers should be read and set.
 * @param param Unused.
 */
void CPU_TBY(struct CPU* CPU, uint16_t param);

/**
 * @brief Transfer the value from Index Register X into Index Register Y.
 * 
 * Reads Index Register X and sets the value into Index Register Y.
 * 
 * @param CPU The CPU whose registers should be read and set.
 * @param param Unused.
 */
void CPU_TXY(struct CPU* CPU, uint16_t param);

/**
 * @brief Transfer the value from Accumulator B into Accumulator A.
 * 
 * Reads Accumulator B and sets the value into Accumulator A.
 * 
 * @param CPU The CPU whose registers should be read and set.
 * @param param Unused.
 */
void CPU_TBA(struct CPU* CPU, uint16_t param);

/**
 * @brief Transfer the value from Index Register X into Accumulator A.
 * 
 * Reads Index Register X and sets the value into Accumulator A.
 * 
 * @param CPU The CPU whose registers should be read and set.
 * @param param Unused.
 */
void CPU_TXA(struct CPU* CPU, uint16_t param);

/**
 * @brief Transfer the value from Index Register Y into Accumulator A.
 * 
 * Reads Index Register Y and sets the value into Accumulator A.
 * 
 * @param CPU The CPU whose registers should be read and set.
 * @param param Unused.
 */
void CPU_TYA(struct CPU* CPU, uint16_t param);

/**
 * @brief Transfer the value from Accumulator A into Accumulator B.
 * 
 * Reads Accumulator A and sets the value into Accumulator B.
 * 
 * @param CPU The CPU whose registers should be read and set.
 * @param param Unused.
 */
void CPU_TAB(struct CPU* CPU, uint16_t param);

/**
 * @brief Transfer the value from Index Register X into Accumulator B.
 * 
 * Reads Index Register X and sets the value into Accumulator B.
 * 
 * @param CPU The CPU whose registers should be read and set.
 * @param param Unused.
 */
void CPU_TXB(struct CPU* CPU, uint16_t param);

/**
 * @brief Transfer the value from Index Register Y into Accumulator B.
 * 
 * Reads Index Register Y and sets the value into Accumulator B.
 * 
 * @param CPU The CPU whose registers should be read and set.
 * @param param Unused.
 */
void CPU_TYB(struct CPU* CPU, uint16_t param);

/**
 * @brief Set the program counter.
 * 
 * Change which instruction the CPU is executing.
 * 
 * @param CPU The CPU whose program counter should be set.
 * @param param The new program counter.
 */
void CPU_JMP(struct CPU* CPU, uint16_t param);

/**
 * @brief Set the program counter.
 * 
 * Change which instruction the CPU is executing from a value in RAM.
 * 
 * @param CPU The CPU whose program counter should be set.
 * @param param The address of the new program counter in RAM.
 */
void CPU_JMP_addr(struct CPU* CPU, uint16_t param);

/**
 * @brief Set the program counter on a condition.
 * 
 * Change which instruction the CPU is executing if the last Compare was equal.
 * 
 * @param CPU The CPU whose program counter should be set.
 * @param param The new program counter.
 */
void CPU_JIE(struct CPU* CPU, uint16_t param);

/**
 * @brief Set the program counter on a condition.
 * 
 * Change which instruction the CPU is executing from a value in RAM if the last Compare was equal.
 * 
 * @param CPU The CPU whose program counter should be set.
 * @param param The address of the new program counter in RAM.
 */
void CPU_JIE_addr(struct CPU* CPI, uint16_t param);

/**
 * @brief Set the program counter on a condition.
 * 
 * Change which instruction the CPU is executing if the last Compare was less than.
 * 
 * @param CPU The CPU whose program counter should be set.
 * @param param The new program counter.
 */
void CPU_JIL(struct CPU* CPU, uint16_t param);

void CPU_JIL_addr(struct CPU* CPU, uint16_t param);

/**
 * @brief Set the program counter on a condition.
 * 
 * Change which instruction the CPU is executing if the last Compare was greater than.
 * 
 * @param CPU The CPU whose program counter should be set.
 * @param param The new program counter.
 */
void CPU_JIG(struct CPU* CPU, uint16_t param);

void CPU_JIG_addr(struct CPU* CPU, uint16_t param);

/**
 * @brief Does nothing for a cycle.
 * 
 * Does absolutely nothing for 1 cycle.
 * 
 * @param CPU The CPU executing the command.
 * @param param Unused.
 */
void CPU_NOP(struct CPU* CPU, uint16_t param);

#endif