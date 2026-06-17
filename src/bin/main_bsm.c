#include "parser.h"
#include "assembler.h"
#include "binary.h"

#include <stdio.h>

#define CHK_ASM_STATUS(prog, status, line) do {\
    if (status != ASM_OK){\
        printAsmStatus(prog, status, line);\
        exit(EXIT_FAILURE);\
    }\
} while (0)



void usage(char *prog){
    printf("%s <file.bsm> <out.bsin>\n", prog);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]){

    if (argc != 3){
        usage(argv[0]);
    }

    printf("Hello world !\n");
        size_t prog_size = 0;
    struct IR_Node *IRList = parser(argv[1], &prog_size);
    printf("prog size : %lu\n", prog_size);
    for (size_t i = 0; i < prog_size; i++){
        printIRNode(&IRList[i]);
    }
    printf("PARSER OK\n\n\n");

    struct Assembler ass = {0};
    AsmError status;

    status = assemblerFirstPass(argv[1], &ass, IRList, prog_size);
    if (status != ASM_OK) CHK_ASM_STATUS(argv[1], status, ass.error_line);

    printf("ASM first pass OK\n\n");
    for (size_t i = 0; i < ass.symtable.next; i++){
        printf("%s : %lu\n", ass.symtable.table[i].string, ass.symtable.table[i].addr);
    }

    printf("\n");
    status = assemblerSecondPass(argv[1], &ass, IRList, prog_size);
    if (status != ASM_OK) CHK_ASM_STATUS(argv[1], status, ass.error_line);
    printf("ASM second pass OK\n\n");

    for (size_t i = 0; i < prog_size; i++){
        free_IRNode(&IRList[i]);
    }
    free(IRList);
    free(ass.symtable.table);
   
    writeBinaryFile(argv[2], &ass.bin);

    free(ass.bin.bytecode);
    free(ass.bin.data);

    exit(EXIT_SUCCESS);
}