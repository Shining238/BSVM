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
    DIR_STRING,
    DIR_VALUE,
    DIR_UNDEF
} DirType;

typedef enum {
    PARSER_UNKNOWN_INSTR,
    PARSER_SYNTAX_ERROR,
    PARSER_TOO_MANY_OPE,
    PARSER_TOO_FEW_OPE,
    PARSER_OK,
    PARSER_EMPTY
} ParserError;

typedef enum {
    OPERAND_NONE,
    OPERAND_VALUE,
    OPERAND_REG,
    OPERAND_LABEL,
    OPERAND_STRING,
    OPERAND_MEM_IDX,
    OPERAND_MEM_STR
} OperandType;

typedef struct {
    OperandType type;
    union {
        uint8_t reg;
        int64_t value;
        char *str;

        struct {
            struct {
                union {
                    char *label;
                    uint8_t base_reg;
                };
            int64_t offset;
            } idx;
        } mem;
    };
 } Operand;

struct IR_Node {
    IRType type;

    union {
        struct {
            Operand name;
        } label;

        struct {
            OP_CODE op;
            MODE mode;
            Operand a;
            Operand b;
        } instr;

        struct {
            DirType type;
            char *op;
            union {
                Operand value;
                Operand string;
            };
        } directive;
    };
};

typedef struct OpMap {
    const char *mnemonic;
    OP_CODE op;
} OpMap;

struct IR_Node *parser(char *filename, size_t *n);

void free_IRNode(struct IR_Node *IR);