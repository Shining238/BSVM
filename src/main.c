#include "common.h"
#include "vm.h"
#include "loader.h"
#include "cpu.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int main(){

    struct VM vm;
    initVM(&vm);

    const struct Instruction i4 = {.opcode = OP_LD, .mode = IMM, .a = 0, .args.imm = 6};
    const struct Instruction i5 = {.opcode = OP_LD, .mode = IMM, .a = 1, .args.imm = 7};
    const struct Instruction i6 = {.opcode = OP_ADDR, .mode = REG, .a = 0, .b = 1};
    const struct Instruction i7 = {.opcode = OP_STD, .mode = IMM, .a = 0, .args.imm = 200};
    const struct Instruction i8 = {.opcode = OP_LD, .mode = IMM, .a = 1, .args.imm = 200};
    const struct Instruction i9 = {.opcode = OP_PUSH, .mode = REG, .a = 1};
    const struct Instruction i10 = {.opcode = OP_CMP, .mode = IMM, .a = 0, .args.imm = 14};
    const struct Instruction i11 = {.opcode = OP_JLT, .mode = IMM, .args.imm = 78};
    const struct Instruction i12 = {.opcode = OP_HALT, .mode = NONE};
    const struct Instruction nop = {.opcode = OP_NOP, .mode = NONE};

    const struct Instruction start[10] = {
        i4,
        i5,
        i6,
        i7,
        i8,
        i9,
        i10,
        i11,
        nop,
        i12
    };

    const struct Instruction i1 = {.opcode = OP_LDI, .mode = REG, .a = 0, .b = 1};
    const struct Instruction i2 = {.opcode = OP_ADD, .mode = IMM, .a = 0, .args.imm = 1};
    const struct Instruction i3 = {.opcode = OP_HALT, .mode = NONE};

    const struct Instruction label1[3] = {
        i1,
        i2,
        i3
    };

    struct Instruction program[13] = {0};
    memcpy(program, start, sizeof(start));
    memcpy(program + 10, label1, sizeof(label1));

    size_t prog_size = totalProgSize(program, 13);
    uint8_t bytecode[DATA_BASE] = {0};
    size_t written = encodeProgram(program, 13, bytecode);

    if (prog_size != written){
        fprintf(stderr, "Erreur lors de l'encodage du programme\n");
        exit(EXIT_FAILURE);
    }

    VM_Error status = loadProgram(&vm, bytecode, written, 0);
    char *error_string;
    if (status == LOAD_OUT_OF_BOUNDS){
        fprintf(stderr, "Program out of bounds\n");
        exit(EXIT_FAILURE);
    }

    while (((status = cpu_step(&vm)) == VM_OK) && vm.running);

    if (status != VM_OK){
        error_string = errorToString(status);
        fprintf(stderr, "%s\n", error_string);
        printVM(&vm);
        free(error_string);
        exit(EXIT_FAILURE);
    }
    else {
        printf("Successfull execution\n");
        printVM(&vm);
    }

    exit(EXIT_SUCCESS);
}