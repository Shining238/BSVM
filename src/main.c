#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "vm.h"
#include "memory.h"
#include "instructions.h"
#include "instructions_codec.h"
#include "loader.h"

#define CHK_OP(instr, op_name, str) ((instr->opcode == op_name) ? str : "OUPS")

#define CHK_MODE(instr, mode_name, str) ((instr->mode == mode_name) ? str : "OUPS")

void usage(char *prog){
    printf("usage : %s <filename>\n", prog);
    exit(1);
}

void test_instr(const struct Instruction *instr, uint8_t *buffer, char *op_str, char *mode_str){

    struct Instruction res = {0};

    size_t written = instructionEncode(instr, buffer, 16);
    size_t consumed = instructionDecode(&res, buffer, 16);
    //char *op_name = CHK_OP(res, instr->opcode, op_str);
    //char *mode = CHK_MODE(res, instr->mode, mode_str);

    printf("%s %s %lu", op_str, mode_str, written);
    if (written != consumed){
        printf("écrit != consommé ");
    }
    if (instr->opcode != res.opcode){
        printf("op_code ");
    }
    if (instr->mode != res.mode){
        printf("mode ");
    }
    if (instr->a != res.a){
        printf("A ");
    }
    if (instr->b != res.b){
        printf("B ");
    }
    if (instr->args.offset != res.args.offset){
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

size_t totalProgSize(const struct Instruction *program, size_t size){
    
    size_t total = 0;
    for (size_t i = 0; i < size; i++){
        total += mode_size[program[i].mode];
    }
    return total;
}


int main(int argc, char *argv[]){

    if (argc != 2){
        usage(argv[0]);
    }

    char *filename = argv[1];

    CHK(access(filename, R_OK | F_OK));

    struct VM vm;

    initVM(&vm);

    int64_t value;
    VM_Error status;

    (void) mode_size;

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

    const struct Instruction instrHALT = {.opcode = OP_HALT, .mode = NONE};

    printf("\n\n-----Test codec-----\n");
    uint8_t buffer[16];

    test_instr(&instrADD, buffer, "ADD", "IMM");
    test_instr(&instrADDR, buffer, "ADDR", "REG");
    test_instr(&instrADDD, buffer, "ADDD", "IMM");
    test_instr(&instrADDI, buffer, "ADDI", "REG");
    test_instr(&instrADDX, buffer, "ADDX", "OFF");

    test_instr(&instrCALLX, buffer, "CALLX", "OFF");

    test_instr(&instrJMPX, buffer, "JMPX", "OFF");

    test_instr(&instrPUSH, buffer, "PUSH", "REG");

    test_instr(&instrRTN, buffer, "RTN", "NONE");

    test_instr(&instrHALT, buffer, "HALT", "NONE");

    const struct Instruction program[16] = {
        instrADD,
        instrADDR,
        instrADDD,
        instrADDI,
        instrADDX,
        instrCALLX,
        instrJMPX,
        instrPUSH,
        instrRTN,
        instrHALT
    };

    uint8_t bytecode[DATA_BASE] = {0};
   
    size_t written = encodeProgram(program, 10, bytecode);

    status = loadProgram(&vm, bytecode, written, 80);
    
    printf("----- Test loader -----\n");
    if (status == VM_OK){
        printVM(&vm);
        printf("taille totale du programme : %lu\n", totalProgSize(program, 10));
        printf("%lu bytes chargés en mémoire\n", written);
    }
    else if (status == LOAD_OUT_OF_BOUNDS){
        printf("Out of bounds : %lu >= %lu\n", written, (size_t) DATA_BASE);
    }

    size_t cursor = 80;
    for (size_t i = 0; i < 10; i++){
        test_instr(&program[i], vm.memory + cursor, "OPCODE", "MODE");
        cursor += mode_size[program[i].mode];
    }

    return 0;
}