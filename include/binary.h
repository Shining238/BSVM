#pragma once

#include <stdint.h>
#include <stdlib.h>

#define MAGIC_NUMBER 0x66838677
#define HDR_SIZE 28

struct BinaryHeader {
    uint32_t magic_number;
    uint64_t entry_point;
    size_t code_size;
    size_t data_size;
};

struct Binary {
    struct BinaryHeader headers;
    uint8_t *bytecode;
    uint8_t *data;
};

void readBinaryFile(char *filename, struct Binary *binary);

void writeBinaryFile(char *filename, struct Binary *binary);