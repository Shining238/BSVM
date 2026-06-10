#pragma once

#include "common.h"
#include "vm.h"
#include <stdlib.h>
#include <stdint.h>

//256 max
typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_CMP,
    OP_NEG,
    OP_AND,
    OP_OR,
    OP_XOR,
    OP_NOT,
    OP_SHL,
    OP_SHR,
    OP_ST,
    OP_LD,
    OP_SWP,
    OP_PUSH,
    OP_POP,
    OP_FAS,
    OP_CALL,
    OP_RTN,
    OP_NOP,
    OP_RST,
    OP_JMP,
    OP_JEQ,
    OP_JNE,
    OP_JLE,
    OP_JLT,
    OP_JGE,
    OP_JGT,
    OP_HALT,
    OP_COUNT
} OP_CODE;

typedef enum {
    MODE_NONE,
    MODE_IMM,
    MODE_REGU,
    MODE_REG,
    MODE_DIR,
    MODE_IND,
    MODE_IDX,
    MODE_COUNT
} MODE;

static size_t mode_size[MODE_COUNT] = {
    [MODE_NONE] = 2,
    [MODE_IMM] = 11,
    [MODE_REGU] = 3,
    [MODE_REG] = 4,
    [MODE_DIR] = 11,
    [MODE_IND] = 4,
    [MODE_IDX] = 12
};

_Static_assert(OP_COUNT <= 256, "Too many op_codes\n");
_Static_assert(MODE_COUNT <= 256, "Too many modes\n");

struct Instruction {
    OP_CODE opcode;
    MODE mode;
    uint8_t a;
    uint8_t b;
    union args {
        int64_t offset;
        int64_t imm;
    } args;
};

typedef VM_Error (*InstrHandler)(struct VM *, const struct Instruction *);

InstrHandler get_handler(OP_CODE op_code, MODE mode);