#include "common.h"
#include "vm.h"
#include "loader.h"
#include "cpu.h"
#include "binary.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int main(){

    struct VM vm;
    initVM(&vm);

    const struct Instruction i4 = {.opcode = OP_LD, .mode = IMM, .a = 0, .args.imm = 6};
    const struct Instruction i5 = {.opcode = OP_LD, .mode = IMM, .a = 1, .args.imm = 7};
    const struct Instruction i6 = {.opcode = OP_ADD, .mode = REG, .a = 0, .b = 1};
    const struct Instruction i7 = {.opcode = OP_ST, .mode = DIR, .a = 0, .args.imm = 200};
    const struct Instruction i8 = {.opcode = OP_LD, .mode = IMM, .a = 1, .args.imm = 200};
    const struct Instruction i9 = {.opcode = OP_PUSH, .mode = REG, .a = 1};
    const struct Instruction i10 = {.opcode = OP_CMP, .mode = IMM, .a = 0, .args.imm = 14};
    const struct Instruction i11 = {.opcode = OP_JLT, .mode = IMM, .args.imm = 0};
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

    const struct Instruction i1 = {.opcode = OP_LD, .mode = IND, .a = 0, .b = 1};
    const struct Instruction i2 = {.opcode = OP_ADD, .mode = IMM, .a = 0, .args.imm = 1};
    const struct Instruction i3 = {.opcode = OP_HALT, .mode = NONE};

    const struct Instruction label1[3] = {
        i1,
        i2,
        i3
    };

    struct Instruction program[13] = {0};
    memcpy(program, label1, sizeof(label1));
    memcpy(program + 3, start, sizeof(start));

    //memcpy(program, start, sizeof(start));
    //memcpy(program+10, label1, sizeof(label1));

    size_t prog_size = totalProgSize(program, 13);
    uint8_t bytecode[DATA_BASE] = {0};
    uint8_t data[STACK_BASE - DATA_BASE] = {0};
    size_t written = encodeProgram(program, 13, bytecode);

    struct BinaryHeader headers = {.magic_number=MAGIC_NUMBER, .entry_point=17, .code_size=written, .data_size=0};
    struct Binary bin = {.headers=headers, .bytecode=bytecode, .data=data};

    writeBinaryFile("out.bsin", &bin);

    uint8_t out_buffer[DATA_BASE];
    struct Binary out = {.bytecode = out_buffer, .data = data};
    readBinaryFile("out.bsin", &out);
    if (out.headers.magic_number != bin.headers.magic_number){
        fprintf(stderr, "Wrong magic number\n");
        exit(EXIT_FAILURE);
    }
    if (out.headers.entry_point != bin.headers.entry_point){
        fprintf(stderr, "Wrong entry point\n");
        exit(EXIT_FAILURE);
    }
    if (out.headers.code_size != bin.headers.code_size){
        fprintf(stderr, "Wrong code size\n");
        exit(EXIT_FAILURE);
    }
    if (out.headers.data_size != bin.headers.data_size){
        fprintf(stderr, "Wrong code size\n");
        exit(EXIT_FAILURE);
    }
 
    for (size_t i = 0; i < out.headers.code_size; i++){
        if (out.bytecode[i] != bin.bytecode[i]){
            fprintf(stderr, "Corruption du bytecode\n");
            exit(EXIT_FAILURE);
        }
    }

    for (size_t i = 0; i < out.headers.data_size; i++){
        if (out.data[i] != bin.data[i]){
            fprintf(stderr, "Corruption du segment data\n");
            exit(EXIT_FAILURE);
        }
    }
    printf("lecture ecriture ok\n");

    if (prog_size != written){
        fprintf(stderr, "Erreur lors de l'encodage du programme %lu != %lu\n", written, prog_size);
        exit(EXIT_FAILURE);
    }

    VM_Error status = loadProgram(&vm, &bin, 0);
    char *error_string;
    if (status != VM_OK){
        error_string = errorToString(status);
        free(error_string);
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