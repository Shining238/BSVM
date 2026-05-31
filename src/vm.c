#include "common.h"
#include "vm.h"

#include <stdio.h>
#include <stdlib.h>

void initVM(struct VM *vm){
    vm->sp = MEM_SIZE;
    vm->pc = 0;
    vm->sr = 0x10;
}

void printVM(struct VM *vm){
    printf("------Affichage------\n");

    printf("--REGISTERS--\n\n");
    for (size_t i = 0; i < N_REG; i++){
        printf("R%lu\t: %x\n", i, vm->registers[i]);
    }
    printf("\nPC : %x\nSP : %x\nSR : %x\n\n", vm->pc, vm->sp, vm->sr);
    printf("--MEMORY--\n\n");

    for (size_t i = 0; i < MEM_SIZE; i++){
        printf("%lx\t: %x\n", i, vm->memory[i]);
    }
    printf("------Fin------\n\n");
}