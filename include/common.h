#pragma once 

#define MEM_SIZE 64
#define N_REG 2

#define CHK(op) do {\
    if (op == -1){\
        perror(#op);\
        exit(1);\
    }\
} while (0)