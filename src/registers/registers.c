#include "../../include/registers/registers.h"

struct RegisterArray Register_create() {
    // Create a new stack instance
    struct RegisterArray registers;
    registers.size = NUM_REG_TYPES;

    // Allocate array, set to correct type and 0 data.
    registers.arr = (struct Register*)malloc(sizeof(struct Register) * NUM_REG_TYPES);

    for (uint8_t i = 0; i < registers.size; i++) {
        registers.arr[i].type = (RegisterType)i;
        registers.arr[i].data = 0;
    }

    return registers;
}

void Register_destroy(struct RegisterArray* registers) {
    // Free and make sure no use-after-frees happen
    free(registers->arr);
    registers->arr = NULL;
}

struct Status Register_bytes_to_status(const uint8_t data) {
    struct Status status = {0};
    // Check every bit and set into status
    status.carry = CHECK_BIT(data, 1);
    status.zero = CHECK_BIT(data, 2);
    status.int_disable = CHECK_BIT(data, 3);
    status.break_command = CHECK_BIT(data, 5);
    status.overflow = CHECK_BIT(data, 6);
    status.negative = CHECK_BIT(data, 7);

    return status;
}

uint8_t Register_status_to_bytes(const struct Status status) {
    // Create mask of each bit in each position
    uint8_t mask = (status.carry << 1) | (status.zero << 2) | (status.int_disable << 3) | (status.break_command << 5) | (status.overflow << 6) | (status.negative << 7);

    return mask;
}

void Register_write(struct RegisterArray* registers, const RegisterType type, const uint8_t data) {
    // Get register and set data
    registers->arr[type].data = data;
}

uint8_t Register_read(const struct RegisterArray* registers, const RegisterType type) {
    // Get register and read data
    return registers->arr[type].data;
}

void Register_write_pc(struct RegisterArray* registers, const uint16_t data) {
    // Create mask of 8 1s and 8 0s
    uint16_t mask = 0xFF00;
    
    // Get left side of data
    uint8_t pc1 = data & mask;

    // Swap mask
    mask = 0x00FF;

    // Get right side of data
    uint8_t pc2 = data & mask;

    // Write each side
    Register_write(registers, PROGRAM_COUNTER1, pc1);
    Register_write(registers, PROGRAM_COUNTER2, pc2);
}

uint16_t Register_read_pc(const struct RegisterArray* registers) {
    // Read each half
    uint8_t pc1 = Register_read(registers, PROGRAM_COUNTER1);
    uint8_t pc2 = Register_read(registers, PROGRAM_COUNTER2);

    // Shift into position and or together
    uint16_t pc = (pc1 << 8) | (pc2);

    return pc;
}

void Register_write_stack_ptr(struct RegisterArray* registers, const uint16_t data) {
    // Create mask of 8 1s and 8 0s
    uint16_t mask = 0xFF00;
    
    // Get left side of data
    uint8_t ptr1 = data & mask;

    // Swap mask
    mask = !mask;

    // Get right side of data
    uint8_t ptr2 = data & mask;

    // Write each side
    Register_write(registers, STACK_POINTER1, ptr1);
    Register_write(registers, STACK_POINTER2, ptr2);
}

uint16_t Register_read_stack_ptr(const struct RegisterArray* registers) {
    // Read each half
    uint8_t ptr1 = Register_read(registers, STACK_POINTER1);
    uint8_t ptr2 = Register_read(registers, STACK_POINTER2);

    // Shift into position and or together
    uint16_t ptr = (ptr1 << 8) | (ptr2);

    return ptr;
}

void Register_write_status(struct RegisterArray* registers, const struct Status data) {
    uint8_t bytes = Register_status_to_bytes(data);

    Register_write(registers, STATUS, bytes);
}

struct Status Register_read_status(const struct RegisterArray* registers) {
    uint8_t bytes = Register_read(registers, STATUS);

    struct Status data = Register_bytes_to_status(bytes);

    return data;
}