#ifndef EMULATOR_CPU_INTERRUPTS_INTERRUPTS_H
#define EMULATOR_CPU_INTERRUPTS_INTERRUPTS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "../CPUutils.h"

/**
 * @struct Interrupts
 * @brief Represents an interrupt in a CPU, whether that be software or hardware.
 * 
 * A node in a linked list of CPU interrupts, that can be regular IRQs or unmaskable NMIs.
 */
struct Interrupt {
    bool maskable; // If the interrupt is maskable, true for IRQ and false to NMI
    Instruction routine; // What to run when it is handled
    uint8_t id; // The id of the interrupt
    struct Interrupt* next; // Next interrupt in queue
};

/**
 * @struct InterruptQueue
 * @brief A queue of interrupts.
 * 
 * Represents the queue of interrupts the CPU needs to handle, only 1 is handled at a time.
 */
struct InterruptQueue {
    struct Interrupt* head; // First node in queue
    uint8_t size; // Amount of nodes in queue
};

/** 
 * @brief Checks if 1 Interrupt is less than the other.
 * 
 * Compares 2 interrupts, with priority over the maskable. If one is maskable and the other is not, the one that is maskable is considered less. If the maskables are the same id is compared.
 * 
 * @param a A pointer to an Interrupt that will be compared against b.
 * @param b A pointer to an Interrupt that will be compared against a.
 * @return True if a is less than b, otherise false.
*/
bool less_than(struct Interrupt* a, struct Interrupt* b);

/**
 * @brief Creates a new Interrupt instance on the stack.
 * 
 * Initialzies a new Interrupt struct instance on the stack, unconnected to other Interrupts.
 * 
 * @param maskable If the Interrupt is maskable.
 * @param routine What to call when the Interrupt is handled.
 * @return A new Interrupt instance on the stack.
 */
struct Interrupt Interrupt_create(bool maskable, Instruction routine);

/**
 * @brief Handles an Interrupt in the CPU and pops it off the queue.
 * 
 * Pops an Interrupt of the queue in a CPU, then calls its handle routine. State of CPU is restored as it was before interrupt was called.
 * 
 * @param CPU The CPU to check for interrupts for.
 */
void Interrupt_handle(struct CPU* CPU);

/**
 * @brief Pushes a new Interrupt onto the queue.
 * 
 * Creates a copy of an Interrupt on the heap and pushes it onto a queue.
 * 
 * @param queue The queue to push an Interrupt onto.
 * @param interrupt The interrupt to make a copy of and push onto the queue.
 */
void Interrupt_push(struct InterruptQueue* queue, struct Interrupt* interrupt);

/**
 * @brief Creates an Interrupt queue on the stack.
 * @warning The return of this function should be passed to Interrupt_destroy_queue when it is no longer needed.
 * @see Interrupt_destroy_queue
 * 
 * Creates an empty Interrupt queue, with NULL pointers and 0 size.
 * 
 * @return A new empty Interrupt queue.
 */
struct InterruptQueue Interrupt_create_queue();

/**
 * @brief Destroys an Interrupt queue.
 * @warning Should only be used when the queue is no longer needed.
 * 
 * Frees all heap nodes in a queue of Interrupts.
 * 
 * @param queue The queue to free memory of.
 */
void Interrupt_destroy_queue(struct InterruptQueue* queue);

#endif