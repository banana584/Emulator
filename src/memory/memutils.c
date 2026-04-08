#include "../../include/memory/memutils.h"

struct BinSize Mem_convert(const struct BinSize current, const DataSize target) {
    // Create new instance with requested units
    struct BinSize new = { target, 0 };

    int8_t difference = target - current.unit;

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

struct BinSize Mem_simplify(const struct BinSize current) {
    struct BinSize new = { current.unit, current.amount };

    // Check if amount can be lowered (if it is over 1024)
    while (new.amount > 1024) {
        if (new.unit + 1 < new.unit) {
            break;
        }
        new = Mem_convert(new, new.unit + 1);
    }

    // Check if amount can be raised (if it is a decimal)
    while (new.amount != round(new.amount)) {
        if (new.unit - 1 > new.unit) {
            break;
        }
        new = Mem_convert(new, new.unit - 1);
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
    printf("%f %s", size.amount, ending);
}