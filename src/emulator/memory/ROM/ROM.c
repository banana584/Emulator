#include "../../../../include/emulator/memory/ROM/ROM.h"

char* strdup (const char* s) {
    size_t slen = strlen(s);
    char* result = malloc(slen + 1);
    if(result == NULL) {
        return NULL;
    }

    memcpy(result, s, slen + 1);
    return result;
}

struct ROM ROM_create(const char* path) {
    // Check for errors when opening binary file
    FILE* file = fopen(path, "rb");
    if (!file) {
        perror("Error opening ROM file");
        return (struct ROM){ path, { BYTE, 0 }, NULL };
    }

    // Get size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Convert size to simplest form
    struct BinSize byte_size = { BYTE, file_size };
    struct BinSize size = Mem_simplify(byte_size);

    // Allocate array and copy values in
    uint8_t* arr = (uint8_t*)malloc(file_size);

    fread(arr, 1, file_size, file);

    // Clean up file after use
    fclose(file);

    // Copy values into new ROM instance
    struct ROM ROM = { strdup(path), size, arr };

    return ROM;
}

void ROM_destroy(struct ROM* ROM) {
    // Destroy data and prevent use-after-free's
    free((void*)ROM->arr);
    free((void*)ROM->path);
    ROM->arr = NULL;
}

uint8_t ROM_read(struct ROM* ROM, uint16_t index) {
    // Get a byte of data at an index
    return ROM->arr[index];
}