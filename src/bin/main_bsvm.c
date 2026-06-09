#include "common.h"
#include "vm.h"
#include "loader.h"
#include "cpu.h"
#include "binary.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void usage(char *prog){
    printf("usage : %s <file.bsin>\n", prog);
    exit(1);
}


int main(int argc, char *argv[]){

    if (argc != 2){
        usage(argv[0]);
    }

    struct VM vm;
    initVM(&vm);

    uint8_t code[DATA_BASE] = {0};
    uint8_t data[STACK_BASE - DATA_BASE] = {0};

    struct Binary bin = {.bytecode=code, .data=data};

    readBinaryFile(argv[1], &bin);

    VM_Error status = loadProgram(&vm, &bin, 0);

    char *error_string;
    if (status != VM_OK){
        error_string = errorToString(status);
        fprintf(stderr, "%s\n", error_string);
        exit(EXIT_FAILURE);
    }

    status = vm_run(&vm);

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