#pragma once
#include <stdint.h>
#include <stdlib.h>
#include "instructions.h"

#define INSTR_LEN 255

typedef enum {
    IR_INSTR,
    IR_LABEL,
    IR_DIR
} IRType;

typedef enum {
    DIR_STRING,
    DIR_ADDR,
    DIR_ENTRY,
    DIR_WORD,
    DIR_COUNT
} DirType;

typedef enum {
    PARSER_INVALID_ARGS,
    PARSER_INVALID_REGISTER,
    PARSER_UNKNOWN_DIR,
    PARSER_UNKNOWN_OP,
    PARSER_UNKNOWN_INSTR,
    PARSER_SYNTAX_ERROR,
    PARSER_TOO_MANY_OPE,
    PARSER_TOO_FEW_OPE,
    PARSER_EMPTY,
    PARSER_OK
} ParserError;

typedef enum {
    OPERAND_NONE,
    OPERAND_VALUE,
    OPERAND_REG,
    OPERAND_IDENT,
    OPERAND_STRING,
    OPERAND_MEM_IDENT,
    OPERAND_MEM_VALUE,
    OPERAND_MEM_REG,
    OPERAND_MEM_IDX,
    OPERAND_MEM_IDX_IDENT
} OperandType;

typedef struct {
    OperandType type;
    union {
        uint8_t reg;
        int64_t value;
        struct {
            char *str;
            size_t length;
        } ident;

        struct {
            union {
                uint8_t base_reg;
                struct {
                    char *str;
                    size_t length;
                } ident;
            };
            int64_t offset;
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
            Operand a;
            Operand b;
        } instr;

        struct {
            DirType type;
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

typedef struct DirMap {
    const char *mnemonic;
    DirType dir;
} DirMap;

struct IR_Node *parser(char *filename, size_t *n);

void printIRNode(struct IR_Node *IR);

void free_IRNode(struct IR_Node *IR);