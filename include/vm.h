#pragma once
#include <stdint.h>
#include <stdlib.h>

#include "common.h"

typedef enum {
    VM_OK,
    VM_DIV_ZERO,
    VM_INSTR_PRIV,
    VM_INSTR_TRAP,
    VM_ILL_MEM_ACCESS,
    VM_ILL_REGISTER,
    VM_STACK_UNDERFLOW,
    VM_STACK_OVERFLOW,
    VM_INVALID_JMP,
    VM_CODE_LOAD_OUT_OF_BOUNDS,
    VM_DATA_LOAD_OUT_OF_BOUNDS,
    VM_INVALID_EXECUTION_ADDRESS,
    VM_TRUNCATED_INSTR,
    VM_UNKNOWN_INSTR,
    VM_UNKNOWN_SYSCALL,
    VM_OUT_OF_THE_HEAP,
    VM_HEAP_OUT_OF_MEMORY,
    VM_MEM_ALREADY_FREE,
    VM_FREE_OUT_OF_THE_HEAP

} VM_Error;


/*       5  4  3  2  1  0
    sr : S IE  O  N  Z  C
    */

struct VM {
    int64_t registers[N_REG];
    uint64_t sp;
    uint64_t pc;
    uint8_t sr;
    uint8_t memory[MEM_SIZE];
    uint8_t running;
    struct MemNode *memblocks;
};

struct MemNode {
    uint64_t begin;
    size_t size;
    struct MemNode *next;
    int free;
};

char *errorToString(VM_Error status);

void initVM(struct VM *vm);

void destroyVM(struct VM *vm);

VM_Error vm_run(struct VM *vm);

void printVM(struct VM *vm);


void setSROF(struct VM *vm, uint8_t value);
uint8_t getSROF(struct VM *vm);

void setSRNF(struct VM *vm, uint8_t value);
uint8_t getSRNF(struct VM *vm);

void setSRZF(struct VM *vm, uint8_t value);
uint8_t getSRZF(struct VM *vm);

void setSRCF(struct VM *vm, uint8_t value);
uint8_t getSRCF(struct VM *vm);