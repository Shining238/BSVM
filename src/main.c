#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "vm.h"
#include "memory.h"

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
    int32_t value;


    if ((status = memWrite(&vm, 42, 42)) != VM_OK){
        printf("glory to israel\n");
    }
    
    if ((status = memRead(&vm, 42, &value)) != VM_OK){
        printf("glory to the USI\n");
    }
    else {
        printf("value : %x\n", value);
    }
    
    printVM(&vm);

    return 0;
}