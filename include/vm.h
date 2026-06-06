#pragma once
#include <stdint.h>

#include "common.h"

typedef enum {
    VM_OK,
    DIV_0,
    INSTR_ILL,
    INSTR_PRIV,
    INSTR_TRAP,
    ILL_MEM_ACCESS,
    ILL_REGISTER,
    STACK_UNDERFLOW,
    STACK_OVERFLOW,
    INVALID_JMP

} VM_Error;

#define M_OVERFLOW 0x08
#define M_NEGATIVE 0x04
#define M_ZERO 0x02
#define M_CARRY 0x01

/*       5  4  3  2  1  0
    sr : S IE  O  N  Z  C
    */

struct VM {
    int64_t registers[N_REG];
    uint64_t sp;
    uint64_t pc;
    uint8_t sr;
    int64_t memory[MEM_SIZE];
    uint8_t running;
};

void initVM(struct VM *vm);

void printVM(struct VM *vm);
