#include <stdio.h>
#include <pthread.h>
#include <ctype.h>
#include "../include/emulator/CPU/CPU.h"
#include "../include/emulator/CPU/interrupts/Interrupts.h"

void Interrupt(struct CPU* CPU, uint16_t param) {
    param = param;

    printf("A = %d, B = %d\n", Register_read(CPU->registers, ACCUMULATOR_A), Register_read(CPU->registers, ACCUMULATOR_B));
}

void keyboard_int(struct CPU* CPU, uint16_t param) {
    printf("Interrupt %d caused stop\n", param);

    CPU->on = false;
}

void* keyboard_thread(void* arg) {
    struct InterruptQueue* queue = (struct InterruptQueue*)arg;

    while (true) {

        char str[1024];

        fgets(str, sizeof(str), stdin);

        char* ptr = str;
        char *end = ptr + strlen(ptr) - 1;

        while (end > ptr && isspace((unsigned char)*end)) {
            *end = '\0';
            end--;
        }

        if (strcmp(ptr, "quit") == 0) {
            struct Interrupt interrupt = Interrupt_create(false, keyboard_int);
            Interrupt_push(queue, &interrupt);

            break;
        } else {
            struct Interrupt interrupt = Interrupt_create(true, Interrupt);
            Interrupt_push(queue, &interrupt);
        }
    }

    return NULL;
}

int main() {
    struct ROM ROM = ROM_create("ROM.bin");
    struct BinSize size = { KILOBYTE, 1 };
    struct Cache cache = Cache_create(size, size);
    size = (struct BinSize){ KILOBYTE, 2 };
    struct RAM RAM = RAM_create(size, &cache);
    struct RegisterArray registers = Register_create();
    struct ClockSpeed speed = { HERTZ, 1 };
    struct Clock clock = Clock_create(speed);
    struct InterruptQueue interrupts = Interrupt_create_queue();

    struct CPU CPU = CPU_create(&RAM, &registers, &ROM, &clock, &interrupts);

    pthread_t thread;
    pthread_create(&thread, NULL, keyboard_thread, (void*)&interrupts);

    CPU_run_limitless(&CPU);

    Interrupt_destroy_queue(&interrupts);
    Register_destroy(&registers);
    RAM_destroy(&RAM);
    Cache_destroy(&cache);
    ROM_destroy(&ROM);

    return 0;
}
