#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "vm.h"
#include "memory.h"
#include "instructions.h"
#include "instructions_codec.h"

#define CHK_OP(instr, op_name, str) ((instr->opcode == op_name) ? str : "OUPS")

#define CHK_MODE(instr, mode_name, str) ((instr->mode == mode_name) ? str : "OUPS")

void usage(char *prog){
    printf("usage : %s <filename>\n", prog);
    exit(1);
}

void test_instr(const struct Instruction *instr, struct Instruction *res, char *op_str, char *mode_str){
    memset(res, 0, sizeof(struct Instruction));

    uint8_t buffer[16];
    size_t written = instruction_encode(instr, buffer);
    size_t consumed = instruction_decode(res, buffer);
    //char *op_name = CHK_OP(res, instr->opcode, op_str);
    //char *mode = CHK_MODE(res, instr->mode, mode_str);

    printf("%s %s %lu", op_str, mode_str, written);
    if (written != consumed){
        printf("écrit != consommé ");
    }
    if (instr->opcode != res->opcode){
        printf("op_code ");
    }
    if (instr->mode != res->mode){
        printf("mode ");
    }
    if (instr->a != res->a){
        printf("A ");
    }
    if (instr->b != res->b){
        printf("B ");
    }
    if (instr->args.offset != res->args.offset){
        printf("args ");
    }
    printf("\n");
    /*
    for (size_t i = 0; i < written; i++){
        printf("%x ", buffer[i]);
    }
    printf("\n");
    printf("w:%lu, r:%lu\nop : %s, mode : %s, R%d, R%d, imm : %lx, offset : %lx\n", written, consumed, op_name, mode, res->a, res->b, res->args.imm, res->args.offset);
*/
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

    (void) mode_size;

    if ((status = memWrite64(&vm, 42, 42)) != VM_OK){
        printf("glory to israel\n");
    }
    
    if ((status = memRead64(&vm, 42, &value)) != VM_OK){
        printf("glory to the USI\n");
    }
    else {
        printf("value : %lx\n", value);
    }
    
    printVM(&vm);

    const struct Instruction instrADD = {.opcode = OP_ADD, .mode = IMM, .a = 0, .args.imm = 0x1234};

    get_handler(OP_ADD)(&vm, &instrADD);

    printf("Test instruction ADD\n");
    printVM(&vm);

    vm.registers[1] = 0x12;
    const struct Instruction instrADDR = {.opcode = OP_ADDR, .mode = REG, .a = 0, .b = 1};

    get_handler(OP_ADDR)(&vm, &instrADDR);

    printf("Test instruction ADDR\n");
    printVM(&vm);

    const struct Instruction instrADDD = {.opcode = OP_ADDD, .mode = IMM, .a = 1, .args.imm = 0x2a};

    get_handler(OP_ADDD)(&vm, &instrADDD);
    
    printf("Test instruction ADDD\n");
    printVM(&vm);

    vm.memory[0x3c] = 2;
    const struct Instruction instrADDI = {.opcode = OP_ADDI, .mode = REG, .a = 0, .b = 1};

    get_handler(OP_ADDI)(&vm, &instrADDI);

    printf("Test instruction ADDI\n");
    printVM(&vm);

    vm.memory[0x39] = 5;
    const struct Instruction instrADDX = {.opcode = OP_ADDX, .mode = OFF, .a = 0, .b = 1, .args.offset = -3};

    printf("addr:%lx\n", (uint64_t) (vm.registers[instrADDX.b] +(int64_t) instrADDX.args.offset));

    memRead64(&vm, (uint64_t) (vm.registers[instrADDX.b] + (int64_t) instrADDX.args.offset), &value);

    printf("value:%lx\n", value);

    get_handler(OP_ADDX)(&vm, &instrADDX);

    printf("Test instruction ADDX\n");
    printVM(&vm);

    vm.registers[1] = 0x2d;
    vm.pc = 40;
    const struct Instruction instrCALLX = {.opcode = OP_CALLX, .mode = OFF, .a = 1, .args.offset = -0x03};

    get_handler(instrCALLX.opcode)(&vm, &instrCALLX);
    printf("Test instruction CALLX\n");
    printVM(&vm);

    const struct Instruction instrRTN = {.opcode = OP_RTN, .mode = NONE};

    get_handler(instrRTN.opcode)(&vm, &instrRTN);
    printf("Test instruction RTN\n");
    printVM(&vm);

    const struct Instruction instrPUSH = {.opcode = OP_PUSH, .mode = REG, .a = 0};
    while ((status = get_handler(instrPUSH.opcode)(&vm, &instrPUSH)) == VM_OK);
    if (status == STACK_OVERFLOW){
        printf("Stack Overflow\n");
    }
    printf("Test STACK_BASE\n");
    printVM(&vm);

    vm.registers[0] = 5;
    const struct Instruction instrJMPX = {.opcode = OP_JMPX, .mode = OFF, .a = 1, .args.offset = -0x2};
    get_handler(instrJMPX.opcode)(&vm, &instrJMPX);

    printf("Test instruction JMPX\n");
    printVM(&vm);

    printf("\n\n-----Test codec-----\n");
    struct Instruction res;

    test_instr(&instrADD, &res, "ADD", "IMM");
    test_instr(&instrADDR, &res, "ADDR", "REG");
    test_instr(&instrADDD, &res, "ADDD", "IMM");
    test_instr(&instrADDI, &res, "ADDI", "REG");
    test_instr(&instrADDX, &res, "ADDX", "OFF");

    test_instr(&instrCALLX, &res, "CALLX", "OFF");

    test_instr(&instrJMPX, &res, "JMPX", "OFF");

    test_instr(&instrPUSH, &res, "PUSH", "REG");

    test_instr(&instrRTN, &res, "RTN", "NONE");
    


    return 0;
}