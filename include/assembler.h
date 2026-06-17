#pragma once

#include "parser.h"
#include "instructions.h"
#include "binary.h"

typedef enum {
    ASM_NOT_A_TEXT_SECTION,
    ASM_NOT_A_DATA_SECTION,
    ASM_SECTION_UNDEFINED,
    ASM_LABEL_REDEFINED,
    ASM_LABEL_UNDEFINED,
    ASM_ENTRY_REDEFINED,
    ASM_WRONG_ARGUMENT_TYPE,
    ASM_TOO_MANY_ARGUMENTS,
    ASM_TOO_FEW_ARGUMENTS,
    ASM_INVALID_ADRESSING_MODE,
    ASM_OK
} AsmError;


struct Symbol {
    const char *string;
    uint64_t addr;
};

struct Symbol_table {
    struct Symbol *table;
    size_t next;
    size_t size;
};

typedef enum {
    SECTION_UNDEF,
    SECTION_TEXT,
    SECTION_DATA
} Section;

struct Assembler {
    struct Symbol_table symtable;
    struct Binary bin;
    size_t error_line;
};



AsmError assemblerFirstPass(char *prog, struct Assembler *ass, const struct IR_Node *IRList, size_t size);

AsmError assemblerSecondPass(char *prog, struct Assembler *ass, const struct IR_Node *IRList, size_t size);

void printAsmStatus(char *prog, AsmError status, size_t line);