#pragma once
#include <stdint.h>

#include "common.h"

typedef enum {
    VM_OK,
    DIV_0,
    INSTR_ILL,
    INSTR_PRIV,
    INSTR_TRAP,
    ILL_MEM_ACCESS
} VM_Error;

/*       5  4  3  2  1  0
    sr : S IE  O  N  Z  C
    */

struct VM {
    int32_t registers[N_REG];
    uint16_t sp;
    uint16_t pc;
    uint8_t sr;
    int32_t memory[MEM_SIZE];
};

void initVM(struct VM *vm);

void printVM(struct VM *vm);
