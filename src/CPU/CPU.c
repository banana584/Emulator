#include "../../include/CPU/CPU.h"
#include "../../include/CPU/interrupts/Interrupts.h"

// Map of every instruction
Instruction instruction_map[256] = {
    CPU_LDA,
    CPU_LDB,
    CPU_LDX,
    CPU_LDY,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    CPU_STA,
    CPU_STB,
    CPU_STX,
    CPU_STY,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    CPU_BRK,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    CPU_CPA,
    CPU_CPB,
    CPU_CPX,
    CPU_CPY,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    CPU_ADD,
    CPU_SUB,
    CPU_MUL,
    CPU_DIV,
    CPU_INC,
    CPU_DEC,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    CPU_PPS,
    CPU_RPS,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    CPU_TAX,
    CPU_TBX,
    CPU_TYX,
    CPU_TAY,
    CPU_TBY,
    CPU_TXY,
    CPU_TBA,
    CPU_TXA,
    CPU_TYA,
    CPU_TAB,
    CPU_TXB,
    CPU_TYB,
    0x0,
    0x0,
    0x0,
    0x0,
    CPU_JMP,
    CPU_JIE,
    CPU_JIL,
    CPU_JIG,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    CPU_NOP,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
};

struct ClockSpeed CPU_convert(const struct ClockSpeed current, CycleAmount target) {
    // Create new instance with requested units
    struct ClockSpeed new = { target, 0 };

    int8_t difference = target - current.unit;

    // Check if we are going up, down or the same
    if (difference > 0) {
        // Target is higher, divide
        uint16_t denominator = 1000 * difference;
        new.amount = current.amount / denominator;
    } else if (difference < 0) {
        // Target is lower, multiply
        uint16_t multiplier = 1000 * difference;
        new.amount = current.amount * multiplier;
    } else {
        // Target is equal, keep the same.
        new.amount = current.amount;
    }

    return new;
}

void CPU_print_clockspeed(const struct ClockSpeed speed) {
    const char* ending = "";

    // Get ending from units
    switch (speed.unit) {
        case HERTZ:
            ending = "hz";
            break;
        case KILOHERTZ:
            ending = "khz";
            break;
        case MEGAHERTZ:
            ending = "mhz";
            break;
        case GIGAHERTZ:
            ending = "ghz";
            break;
        default:
            ending = "null";
    }

    // Print with specified format
    printf("%f %s", speed.amount, ending);
}

struct Clock Clock_create(const struct ClockSpeed target) {
    // Convert target to hertz and set start to NULL
    struct Clock clock = { CPU_convert(target, HERTZ), (time_t)-1 };

    return clock;
}

void Clock_start(struct Clock* clock) {
    // Set start time
    clock->start = time(NULL);
}

void Clock_end(struct Clock* clock) {
    // Get end time and time elapsed
    time_t end = time(NULL);
    time_t elapsed = end - clock->start;

    // Calculate delay and wait that time
    time_t delay = 1 / clock->target.amount - elapsed;

    if (delay > 0) {
        sleep(delay);
    }
}

struct CPU CPU_create(struct RAM* RAM, struct RegisterArray* registers, struct ROM* ROM, struct Clock* clock, struct InterruptQueue* interrupts) {
    // Create CPU with correct components and set to on
    struct CPU CPU = { RAM, registers, ROM, clock, interrupts, true };

    return CPU;
}

void CPU_run_cycle(struct CPU* CPU) {
    // Start clock timing
    Clock_start(CPU->clock);

    // Read program counter and check for program end
    uint16_t pc = Register_read_pc(CPU->registers);

    if (pc > Mem_convert(CPU->ROM->size, BYTE).amount) {
        CPU->on = false;
        return;
    }

    // Read opcode and convert to instruction
    uint8_t opcode = ROM_read(CPU->ROM, pc);
    Instruction instruction = instruction_map[opcode];
    pc += 1;

    // Read left part of arg
    uint8_t left = ROM_read(CPU->ROM, pc);
    pc += 1;

    // Read right part of arg
    uint8_t right = ROM_read(CPU->ROM, pc);
    pc += 1;

    // Combine left and right
    uint16_t arg = 0;
    arg = arg | (left << 8);
    arg = arg | (right);

    // Execute instruction
    instruction(CPU, arg);

    // If jumping get program counter
    if (opcode == 0x70 || opcode == 0x71 || opcode == 0x72 || opcode == 0x73) {
        pc = Register_read_pc(CPU->registers);
    }

    // Update program counter
    Register_write_pc(CPU->registers, pc);

    // Check for interrupts
    Interrupt_handle(CPU);

    printf("A = %d B = %d\n", Register_read(CPU->registers, ACCUMULATOR_A), Register_read(CPU->registers, ACCUMULATOR_B));

    // End clock timing
    Clock_end(CPU->clock);
}

void CPU_run(struct CPU* CPU, const uint64_t max_cycles) {
    // Loop over each cycle
    for (uint64_t cycle = 0; cycle < max_cycles; cycle++) {
        // Check if CPU was turned off
        if (!CPU->on) {
            return;
        }
        // Run a cycle
        CPU_run_cycle(CPU);
    }
}

void CPU_run_limitless(struct CPU* CPU) {
    // Loop forever
    while (true) {
        // Check if CPU was turned off
        if (!CPU->on) {
            return;
        }
        // Run a cycle
        CPU_run_cycle(CPU);
    }
}

// ===== Instructions =====

void CPU_LDA(struct CPU* CPU, uint16_t param) {
    // Write param into A
    Register_write(CPU->registers, ACCUMULATOR_A, param);
}

void CPU_LDB(struct CPU* CPU, uint16_t param) {
    // Write param into B
    Register_write(CPU->registers, ACCUMULATOR_B, param);
}

void CPU_LDX(struct CPU* CPU, uint16_t param) {
    // Write param into X
    Register_write(CPU->registers, X_REG, param);
}

void CPU_LDY(struct CPU* CPU, uint16_t param) {
    // Write param into Y
    Register_write(CPU->registers, Y_REG, param);
}

void CPU_STA(struct CPU* CPU, uint16_t param) {
    param = param;

    // Get stack pointer and move forward
    uint16_t stack_ptr = Register_read_stack_ptr(CPU->registers);
    stack_ptr++;

    // Write into RAM at stack
    RAM_write(CPU->RAM, stack_ptr, Register_read(CPU->registers, ACCUMULATOR_A));

    // Write stack pointer back
    Register_write_stack_ptr(CPU->registers, stack_ptr);
}

void CPU_STB(struct CPU* CPU, uint16_t param) {
    param = param;

    // Get stack pointer and move forward
    uint16_t stack_ptr = Register_read_stack_ptr(CPU->registers);
    stack_ptr++;

    // Write into RAM at stack
    RAM_write(CPU->RAM, stack_ptr, Register_read(CPU->registers, ACCUMULATOR_B));

    // Write stack pointer back
    Register_write_stack_ptr(CPU->registers, stack_ptr);
}

void CPU_STX(struct CPU* CPU, uint16_t param) {
    param = param;

    // Get stack pointer and move forward
    uint16_t stack_ptr = Register_read_stack_ptr(CPU->registers);
    stack_ptr++;

    // Write into RAM at stack
    RAM_write(CPU->RAM, stack_ptr, Register_read(CPU->registers, X_REG));

    // Write stack pointer back
    Register_write_stack_ptr(CPU->registers, stack_ptr);
}

void CPU_STY(struct CPU* CPU, uint16_t param) {
    param = param;

    // Get stack pointer and move forward
    uint16_t stack_ptr = Register_read_stack_ptr(CPU->registers);
    stack_ptr++;

    // Write into RAM at stack
    RAM_write(CPU->RAM, stack_ptr, Register_read(CPU->registers, Y_REG));

    // Write stack pointer back
    Register_write_stack_ptr(CPU->registers, stack_ptr);
}

void CPU_BRK(struct CPU* CPU, uint16_t param) {
    param = param;

    // Get program counter and move forward 2 bytes
    uint16_t pc = Register_read_pc(CPU->registers);
    pc = pc + 2;

    // Get status and set break
    struct Status status = Register_read_status(CPU->registers);
    status.break_command = true;

    // Write into stack
    uint16_t stack_ptr = Register_read_stack_ptr(CPU->registers);
    stack_ptr++;

    RAM_write(CPU->RAM, stack_ptr++, pc);
    RAM_write(CPU->RAM, stack_ptr++, Register_status_to_bytes(status));
}

void CPU_CPA(struct CPU* CPU, uint16_t param) {
    // Get value in A
    uint16_t value = Register_read(CPU->registers, ACCUMULATOR_A);

    struct Status status = Register_read_status(CPU->registers);

    // Compare and set unique states in status
    if (value < param) {
        status.negative = (value >> 7) & 0xFF;
        status.zero = 0;
        status.carry = 0;
    } else if (value == param) {
        status.negative = 0;
        status.zero = 1;
        status.carry = 1;
    } else if (value > param) {
        status.negative = (value >> 7) & 0xFF;
        status.zero = 0;
        status.carry = 1;
    }

    // Write status back
    Register_write_status(CPU->registers, status);
}

void CPU_CPB(struct CPU* CPU, uint16_t param) {
    // Get value in B
    uint16_t value = Register_read(CPU->registers, ACCUMULATOR_B);

    struct Status status = Register_read_status(CPU->registers);

    // Compare and set unique states in status
    if (value < param) {
        status.negative = (value >> 7) & 0xFF;
        status.zero = 0;
        status.carry = 0;
    } else if (value == param) {
        status.negative = 0;
        status.zero = 1;
        status.carry = 1;
    } else if (value > param) {
        status.negative = (value >> 7) & 0xFF;
        status.zero = 0;
        status.carry = 1;
    }

    // Write status back
    Register_write_status(CPU->registers, status);
}

void CPU_CPX(struct CPU* CPU, uint16_t param) {
    // Get value in X
    uint16_t value = Register_read(CPU->registers, X_REG);

    struct Status status = Register_read_status(CPU->registers);

    // Compare and set unique states in status
    if (value < param) {
        status.negative = (value >> 7) & 0xFF;
        status.zero = 0;
        status.carry = 0;
    } else if (value == param) {
        status.negative = 0;
        status.zero = 1;
        status.carry = 1;
    } else if (value > param) {
        status.negative = (value >> 7) & 0xFF;
        status.zero = 0;
        status.carry = 1;
    }

    // Write status back
    Register_write_status(CPU->registers, status);
}

void CPU_CPY(struct CPU* CPU, uint16_t param) {
    // Get value in Y
    uint16_t value = Register_read(CPU->registers, Y_REG);

    struct Status status = Register_read_status(CPU->registers);

    // Compare and set unique states in status
    if (value < param) {
        status.negative = (value >> 7) & 0xFF;
        status.zero = 0;
        status.carry = 0;
    } else if (value == param) {
        status.negative = 0;
        status.zero = 1;
        status.carry = 1;
    } else if (value > param) {
        status.negative = (value >> 7) & 0xFF;
        status.zero = 0;
        status.carry = 1;
    }

    // Write status back
    Register_write_status(CPU->registers, status);
}

void CPU_ADD(struct CPU* CPU, uint16_t param) {
    param = param;

    struct Status status = Register_read_status(CPU->registers);

    // Read varaibles for addition
    uint16_t A = Register_read(CPU->registers, ACCUMULATOR_A);
    uint16_t B = Register_read(CPU->registers, ACCUMULATOR_B);
    uint16_t C = status.carry;

    // Perform addition and set flags
    uint16_t res = A + B + C;
    uint16_t carry = res >> 15;
    uint16_t overflow = ~(A ^ B) & (A ^ res) & 0x80;
    uint16_t zero = CHECK_BIT(res, 7);
    uint16_t negative = res == 0;

    // Set flags in status
    status.carry = carry;
    status.overflow = overflow;
    status.zero = zero;
    status.negative = negative;

    // Write result back
    Register_write(CPU->registers, ACCUMULATOR_A, res);
    Register_write_status(CPU->registers, status);
}

void CPU_SUB(struct CPU* CPU, uint16_t param) {
    // Perform addition with inverted B - same as subtraction
    uint16_t B = Register_read(CPU->registers, ACCUMULATOR_B);

    B = ~B;

    Register_write(CPU->registers, ACCUMULATOR_B, B);

    CPU_ADD(CPU, param);
}

void CPU_MUL(struct CPU* CPU, uint16_t param) {
    param = param;

    // Get variables needed for multiplication
    uint16_t res = 0;
    uint16_t shift = 0;

    uint16_t A = Register_read(CPU->registers, ACCUMULATOR_A);
    uint16_t B = Register_read(CPU->registers, ACCUMULATOR_B);

    uint16_t zero = 0;
    uint16_t overflow = 0;
    uint16_t carry = 0;

    // Loop over and multiply with shifts and XORs
    while (B > 0) {
        if (B & 1) {
            res ^= A << shift;
            carry = 1;
        }
        B >>= 1;
        shift++;

        if (res >= UINT16_MAX) {
            overflow = 1;
            res = 0;
        }
    }

    zero = res == 0;

    // Set status
    struct Status status = Register_read_status(CPU->registers);
    status.zero = zero;
    status.overflow = overflow;
    status.carry = carry;

    // Write result back
    Register_write_status(CPU->registers, status);
    Register_write(CPU->registers, ACCUMULATOR_A, res);
}

void CPU_DIV(struct CPU* CPU, uint16_t param) {
    param = param;

    // Get variables needed for division
    uint16_t res = 0;

    uint16_t A = Register_read(CPU->registers, ACCUMULATOR_A);
    uint16_t B = Register_read(CPU->registers, ACCUMULATOR_B);

    uint16_t remainder = B;

    uint16_t zero = 0;
    uint16_t overflow = 0;
    uint16_t carry = 0;

    // Loop over and divide with shifts
    while (remainder >= A) {
        if ((remainder & 0x80) != 0) {
            overflow = 1;
            remainder = (remainder << 1) - B;
        } else {
            remainder = (remainder << 1);
        }

        res++;
    }

    zero = (A == 0 || B == 0);

    // Set status flags
    struct Status status = Register_read_status(CPU->registers);
    status.zero = zero;
    status.overflow = overflow;
    status.carry = carry;

    // Write result back
    Register_write_status(CPU->registers, status);
    Register_write(CPU->registers, ACCUMULATOR_A, res);
}

void CPU_INC(struct CPU* CPU, uint16_t param) {
    // Read value at address
    uint8_t value = RAM_read(CPU->RAM, param);

    // Add one and write back
    value++;

    RAM_write(CPU->RAM, param, value);

    // Set status flags
    struct Status status = Register_read_status(CPU->registers);
    status.negative = CHECK_BIT(value, 7);
    status.zero = value == 0;
    Register_write_status(CPU->registers, status);
}

void CPU_DEC(struct CPU* CPU, uint16_t param) {
    // Read value at address
    uint8_t value = RAM_read(CPU->RAM, param);

    // Subtract one and write back
    value--;

    RAM_write(CPU->RAM, param, value);

    // Set status flags
    struct Status status = Register_read_status(CPU->registers);
    status.negative = CHECK_BIT(value, 7);
    status.zero = value == 0;
    Register_write_status(CPU->registers, status);
}

void CPU_PPS(struct CPU* CPU, uint16_t param) {
    param = param;

    // Get stack pointer and move forward
    uint16_t stack_ptr = Register_read_stack_ptr(CPU->registers);
    stack_ptr++;

    // Write program counter on stack
    uint16_t pc = Register_read_pc(CPU->registers);
    RAM_write(CPU->RAM, stack_ptr++, pc);

    // Write status on stack
    struct Status status = Register_read_status(CPU->registers);
    uint8_t byte = Register_status_to_bytes(status);
    RAM_write(CPU->RAM, stack_ptr, byte);

    // Write stack pointer back
    Register_write_stack_ptr(CPU->registers, stack_ptr);
}

void CPU_RPS(struct CPU* CPU, uint16_t param) {
    param = param;

    // Get stack pointer
    uint16_t stack_ptr = Register_read_stack_ptr(CPU->registers);

    // Read program counter from stack
    uint16_t pc = RAM_read(CPU->RAM, stack_ptr--);

    // Read byte from stack and convert to status
    uint8_t byte = RAM_read(CPU->RAM, stack_ptr--);
    struct Status status = Register_bytes_to_status(byte);

    // Write back into CPU
    Register_write_pc(CPU->registers, pc);
    Register_write_status(CPU->registers, status);

    // Write stack pointer back
    Register_write_stack_ptr(CPU->registers, stack_ptr);
}

void CPU_TAX(struct CPU* CPU, uint16_t param) {
    param = param;

    // Read A and write to X
    Register_write(CPU->registers, X_REG, Register_read(CPU->registers, ACCUMULATOR_A));
}

void CPU_TBX(struct CPU* CPU, uint16_t param) {
    param = param;

    // Read B and write to X
    Register_write(CPU->registers, X_REG, Register_read(CPU->registers, ACCUMULATOR_B));
}

void CPU_TYX(struct CPU* CPU, uint16_t param) {
    param = param;

    // Read Y and write to X
    Register_write(CPU->registers, X_REG, Register_read(CPU->registers, Y_REG));
}

void CPU_TAY(struct CPU* CPU, uint16_t param) {
    param = param;

    // Read A and write to Y
    Register_write(CPU->registers, Y_REG, Register_read(CPU->registers, ACCUMULATOR_A));
}

void CPU_TBY(struct CPU* CPU, uint16_t param) {
    param = param;

    // Read B and write to Y
    Register_write(CPU->registers, Y_REG, Register_read(CPU->registers, ACCUMULATOR_B));
}

void CPU_TXY(struct CPU* CPU, uint16_t param) {
    param = param;

    // Read X and write to Y
    Register_write(CPU->registers, Y_REG, Register_read(CPU->registers, X_REG));
}

void CPU_TBA(struct CPU* CPU, uint16_t param) {
    param = param;

    // Read B and write to A
    Register_write(CPU->registers, ACCUMULATOR_A, Register_read(CPU->registers, ACCUMULATOR_B));
}

void CPU_TXA(struct CPU* CPU, uint16_t param) {
    param = param;

    // Read X and write to A
    Register_write(CPU->registers, ACCUMULATOR_A, Register_read(CPU->registers, X_REG));
}

void CPU_TYA(struct CPU* CPU, uint16_t param) {
    param = param;

    // Read Y and write to A
    Register_write(CPU->registers, ACCUMULATOR_A, Register_read(CPU->registers, Y_REG));
}

void CPU_TAB(struct CPU* CPU, uint16_t param) {
    param = param;

    // Read A and write to B
    Register_write(CPU->registers, ACCUMULATOR_B, Register_read(CPU->registers, ACCUMULATOR_A));
}

void CPU_TXB(struct CPU* CPU, uint16_t param) {
    param = param;

    // Read X and write to B
    Register_write(CPU->registers, ACCUMULATOR_B, Register_read(CPU->registers, X_REG));
}

void CPU_TYB(struct CPU* CPU, uint16_t param) {
    param = param;

    // Read Y and write to B
    Register_write(CPU->registers, ACCUMULATOR_B, Register_read(CPU->registers, Y_REG));
}

void CPU_JMP(struct CPU* CPU, uint16_t param) {
    // Set program counter
    Register_write_pc(CPU->registers, param);
}

void CPU_JIE(struct CPU* CPU, uint16_t param) {
    struct Status status = Register_read_status(CPU->registers);

    // Zero is only set if last compare was equal, therefore we jump
    if (status.zero) {
        Register_write_pc(CPU->registers, param);
    }
}

void CPU_JIL(struct CPU* CPU, uint16_t param) {
    struct Status status = Register_read_status(CPU->registers);

    // Carry is only 0 if last compare was less than, therefore we jump
    if (!status.carry) {
        Register_write_pc(CPU->registers, param);
    }
}

void CPU_JIG(struct CPU* CPU, uint16_t param) {
    struct Status status = Register_read_status(CPU->registers);

    // Zero is only 0 and carry is set if last compare was greater than, therefore we jump
    if (!status.zero && status.carry) {
        Register_write_pc(CPU->registers, param);
    }
}

void CPU_NOP(struct CPU* CPU, uint16_t param) {
    CPU = CPU;
    param = param;

    // Do nothing for the cycle

    return;
}