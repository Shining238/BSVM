#pragma once

#include <stdint.h>
#include <stdlib.h>

#define MAGIC_NUMBER 0x66838677

struct BinaryHeader {
    uint32_t magic_number;
    uint64_t entry_point;
    size_t code_size;
};

struct Binary {
    struct BinaryHeader headers;
    uint8_t *bytecode;
};

void readBinaryFile(char *filename, struct Binary *binary);

void writeBinaryFile(char *filename, struct Binary *binary);