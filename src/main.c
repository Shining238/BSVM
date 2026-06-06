#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "vm.h"
#include "memory.h"
#include "instructions.h"

void usage(char *prog){
    printf("usage : %s <filename>\n", prog);
    exit(1);
}


int main(int argc, char *argv[]){

    if (argc != 2){
        usage(argv[0]);
    }

    char *filename = argv[1];

    CHK(access(filename, R_OK | F_OK));

    struct VM vm;

    initVM(&vm);

    VM_Error status;
    int64_t value;


    if ((status = memWrite(&vm, 42, 42)) != VM_OK){
        printf("glory to israel\n");
    }
    
    if ((status = memRead(&vm, 42, &value)) != VM_OK){
        printf("glory to the USI\n");
    }
    else {
        printf("value : %lx\n", value);
    }
    
    printVM(&vm);

    struct Instruction instrADD = {.opcode = OP_ADD, .a = 0, .args.imm = 0x1234};

    get_handler(OP_ADD)(&vm, &instrADD);

    printf("Test instruction ADD\n");
    printVM(&vm);

    vm.registers[1] = 0x12;
    struct Instruction instrADDR = {.opcode = OP_ADDR, .a = 0, .args.reg.b = 1};

    get_handler(OP_ADDR)(&vm, &instrADDR);

    printf("Test instruction ADDR\n");
    printVM(&vm);

    struct Instruction instrADDD = {.opcode = OP_ADDD, .a = 1, .args.imm = 0x2a};

    get_handler(OP_ADDD)(&vm, &instrADDD);
    
    printf("Test instruction ADDD\n");
    printVM(&vm);

    vm.memory[0x3c] = 2;
    struct Instruction instrADDI = {.opcode = OP_ADDI, .a = 0, .args.reg.b = 1};

    get_handler(OP_ADDI)(&vm, &instrADDI);

    printf("Test instruction ADDI\n");
    printVM(&vm);

    vm.memory[0x39] = 5;
    struct Instruction instrADDX = {.opcode = OP_ADDX, .a = 0, .args.reg.b = 1, .args.reg.offset = -3};

    printf("addr:%lx\n", (uint64_t) (vm.registers[instrADDX.args.reg.b] +(int64_t) instrADDX.args.reg.offset));

    memRead(&vm, (uint64_t) (vm.registers[instrADDX.args.reg.b] + (int64_t) instrADDX.args.reg.offset), &value);

    printf("value:%lx\n", value);

    get_handler(OP_ADDX)(&vm, &instrADDX);

    printf("Test instruction ADDX\n");
    printVM(&vm);

    vm.registers[1] = 0x2d;
    vm.pc = 40;
    struct Instruction instrCALLX = {.opcode = OP_CALLX, .a = 1, .args.reg.offset = -0x03};

    get_handler(instrCALLX.opcode)(&vm, &instrCALLX);
    printf("Test instruction CALLX\n");
    printVM(&vm);

    struct Instruction instrRTN = {.opcode = OP_RTN};

    get_handler(instrRTN.opcode)(&vm, &instrRTN);
    printf("Test instruction RTN\n");
    printVM(&vm);

    struct Instruction instrPUSH = {.opcode = OP_PUSH, .a = 0};
    while ((status = get_handler(instrPUSH.opcode)(&vm, &instrPUSH)) == VM_OK);
    if (status == STACK_OVERFLOW){
        printf("Stack Overflow\n");
    }
    printf("Test STACK_BASE\n");
    printVM(&vm);

    vm.registers[0] = 5;
    struct Instruction instrJMPX = {.opcode = OP_JMPX, .a = 1, .args.reg.offset = -0x2};
    get_handler(instrJMPX.opcode)(&vm, &instrJMPX);

    printf("Test instruction JMPX\n");
    printVM(&vm);

    return 0;
}