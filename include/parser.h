#pragma once
#include <stdint.h>
#include <stdlib.h>
#include "instructions.h"

#define INSTR_LEN 255

typedef enum {
    IR_INSTR,
    IR_LABEL,
    IR_DIR,
    IR_UNDEF
} IRType;

typedef enum {
    DIR_WORD,
    DIR_STRING,
    DIR_ADDR,
    DIR_UNDEF
} DirType;

typedef enum {
    PARSER_INVALID_TYPE,
    PARSER_INVALID_OP,
    PARSER_OK
} ParserError;

typedef enum {
    OPERAND_NONE,
    OPERAND_IMM,
    OPERAND_REG,
    OPERAND_LABEL,
    OP_MEM
} OperandType;

typedef struct {
    OperandType type;
    union {
        uint8_t reg;
        int64_t imm;
        char label[64];

        struct {
            uint8_t base_reg;
            int64_t offset;
        } mem;
    };
} Operand;

struct IR_Node {
    IRType type;

    union {
        struct {
            char name[64];
        } label;

        struct {
            OP_CODE op;
            Operand a;
            Operand b;
        } instr;

        struct {
            DirType type;
            union {
                char str[128];
                int64_t word;
                uint64_t addr;
            };
        } directive;
    };
};

typedef struct OpMap {
    const char *mnemonic;
    OP_CODE op;
} OpMap;

struct IR_Node *parser(char *filename);