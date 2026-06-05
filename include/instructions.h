#pragma once

#include "common.h"
#include "vm.h"
#include <stdint.h>

//256 max
typedef enum {
    OP_ADD,
    OP_ADDR,
    OP_ADDD,
    OP_ADDI,
    OP_ADDX,
    OP_SUB,
    OP_SUBR,
    OP_SUBD,
    OP_SUBI,
    OP_SUBX,
    OP_MUL,
    OP_MULR,
    OP_MULD,
    OP_MULI,
    OP_MULX,
    OP_DIV,
    OP_DIVR,
    OP_DIVD,
    OP_DIVI,
    OP_DIVX,
    OP_STD,
    OP_STI,
    OP_STX,
    OP_LD,
    OP_LDR,
    OP_LDD,
    OP_LDI,
    OP_LDX,
    OP_PUSH,
    OP_POP,
    OP_NOP,
    OP_COUNT
} OP_CODE;

_Static_assert(OP_COUNT <= 256, "Too many op_codes");

struct Instruction {
    OP_CODE opcode;
    uint8_t a;
    union args {
        struct reg{
            uint8_t b;
            int32_t offset;
        } reg;
        int32_t imm;
    } args;
};

typedef VM_Error (*InstrHandler)(struct VM *, const struct Instruction *);

InstrHandler get_handler(OP_CODE op_code);