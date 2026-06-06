#pragma once 

#define MEM_SIZE 64 // < 2^32
#define STACK_BASE 32
#define DATA_BASE 16
#define N_REG 2 // < 256

#define CHK(op) do {\
    if (op == -1){\
        perror(#op);\
        exit(1);\
    }\
} while (0)