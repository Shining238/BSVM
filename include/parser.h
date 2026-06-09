#pragma once
#include <stdint.h>
#include <stdlib.h>

typedef enum {
    IR_INSTR,
    IR_LABEL,
    IR_DIR
} IRType;

typedef enum {
    WORD,
    STRING,
    ADDR
} DirType;

struct IR_Node {
    IRType type;

    union {
        struct {
            char name[64];
        } label;

        struct {
            char op[16];
            char arg1[64];
            char arg2[64];
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

struct IR_Node *parser(char *filename);