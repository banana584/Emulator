#include "../../../../include/emulator/CPU/interrupts/Interrupts.h"
#include "../../../../include/emulator/CPU/CPU.h"

bool less_than(struct Interrupt* a, struct Interrupt* b) {
    // Maskability has priority over id
    if (a->maskable != b->maskable) {
        // Unmaskable Interrupts are more important
        return a->maskable > b->maskable;
    }
    return a->id < b->id;
}

struct Interrupt Interrupt_create(bool maskable, Instruction routine) {
    // Copy values in and make it unlinked
    struct Interrupt interrupt = { maskable, routine, 0, NULL };

    return interrupt;
}

void Interrupt_handle(struct CPU* CPU) {
    // Get head of queue
    struct Interrupt* interrupt = CPU->interrupts->head;

    // Null check
    if (!interrupt || CPU->interrupts->size == 0) {
        return;
    }

    // If it is maskable and CPU Interrupt disable flag is on, skip for now
    if (Register_read_status(CPU->registers).int_disable && interrupt->maskable) {
        return;
    }

    // Preserve status
    CPU_PPS(CPU, 0);

    // Call routine and set head to next interrupt
    interrupt->routine(CPU, interrupt->id);

    CPU->interrupts->head = interrupt->next;

    free(interrupt);

    CPU->interrupts->size--;

    // Get status back
    CPU_RPS(CPU, 0);
}

void Interrupt_push(struct InterruptQueue* queue, struct Interrupt* interrupt) {
    // Create a copy of Interrupt arg
    struct Interrupt* new = (struct Interrupt*)malloc(sizeof(struct Interrupt));
    new->maskable = interrupt->maskable;
    new->routine = interrupt->routine;
    new->next = NULL;
    new->id = queue->size;

    // Increase size
    queue->size++;

    // If there is no head, or the new node is less than the head insert at beginning
    if (!queue->head || less_than(new, queue->head)) {
        new->next = queue->head;
        queue->head = new;
        return;
    }

    // Otherwise linear search for correct position
    struct Interrupt* current = queue->head;
    while (current->next && !less_than(new, current->next)) {
        current = current->next;
    }

    new->next = current->next;
    current->next = new;
}

struct InterruptQueue Interrupt_create_queue() {
    // Create queue with empty head
    struct InterruptQueue queue = { NULL, 0 };

    return queue;
}

void Interrupt_destroy_queue(struct InterruptQueue* queue) {
    // Loop over each element and free
    struct Interrupt* current = queue->head;
    while (current) {
        struct Interrupt* tmp = current->next;
        free(current);
        current = tmp;
    }

    // Set values back to represent emptiness
    queue->head = NULL;
    queue->size = 0;
}