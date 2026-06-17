#pragma once 

#define MEM_SIZE 2048 // < 2^64
#define STACK_BASE 1536
#define HEAP_BASE 1280
#define DATA_BASE 1024
#define N_REG 2 // < 256

#define CHK(op) do {\
    if (op == -1){\
        perror(#op);\
        exit(1);\
    }\
} while (0)