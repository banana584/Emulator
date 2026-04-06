#include "../../include/memory/memutils.h"

struct BinSize Mem_convert(const struct BinSize current, const DataSize target) {
    // Create new instance with requested units
    struct BinSize new = { target, 0 };

    uint8_t difference = target - current.unit;

    // Check if we are going up, down or the same
    if (difference > 0) {
        // Target is higher, divide
        uint16_t denominator = 1024 * difference;
        new.amount = current.amount / denominator;
    } else if (difference < 0) {
        // Target is lower, multiply
        uint16_t multiplier = 1024 * difference;
        new.amount = current.amount * multiplier;
    } else {
        // Target is equal, keep the same.
        new.amount = current.amount;
    }

    return new;
}

void Mem_print(const struct BinSize size) {
    const char* ending = "";

    // Get ending from units
    switch (size.unit) {
        case BYTE:
            ending = "b";
            break;
        case KILOBYTE:
            ending = "kb";
            break;
        case MEGABYTE:
            ending = "mb";
            break;
        case GIGABYTE:
            ending = "gb";
            break;
        default:
            ending = "null";
    }

    // Print with specified format
    printf("%d %s", size.amount, ending);
}