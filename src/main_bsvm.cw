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