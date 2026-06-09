#pragma once 

#define MEM_SIZE 512 // < 2^64
#define STACK_BASE 256
#define DATA_BASE 128
#define N_REG 2 // < 256

#define CHK(op) do {\
    if (op == -1){\
        perror(#op);\
        exit(1);\
    }\
} while (0)