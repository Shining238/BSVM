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
    OP_NEG,
    OP_AND,
    OP_ANDR,
    OP_ANDD,
    OP_ANDI,
    OP_ANDX,
    OP_OR,
    OP_ORR,
    OP_ORD,
    OP_ORI,
    OP_ORX,
    OP_XOR,
    OP_XORR,
    OP_XORD,
    OP_XORI,
    OP_XORX,
    OP_NOT,
    OP_SHL,
    OP_SHLR,
    OP_SHLD,
    OP_SHLI,
    OP_SHLX,
    OP_SHR,
    OP_SHRR,
    OP_SHRD,
    OP_SHRI,
    OP_SHRX,
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
    OP_FASD,
    OP_FASI,
    OP_FASX,
    OP_CALL,
    OP_CALLR,
    OP_CALLD,
    OP_CALLI,
    OP_CALLX,
    OP_RTN,
    OP_NOP,
    OP_RST,
    OP_JMP,
    OP_JMPR,
    OP_JMPD,
    OP_JMPI,
    OP_JMPX,
    OP_COUNT
} OP_CODE;

_Static_assert(OP_COUNT <= 256, "Too many op_codes");

struct Instruction {
    OP_CODE opcode;
    uint8_t a;
    union args {
        struct reg{
            uint8_t b;
            int64_t offset;
        } reg;
        int64_t imm;
    } args;
};

typedef VM_Error (*InstrHandler)(struct VM *, const struct Instruction *);

InstrHandler get_handler(OP_CODE op_code);