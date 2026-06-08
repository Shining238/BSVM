#include "common.h"
#include "vm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define M_OVERFLOW 0x08
#define M_NEGATIVE 0x04
#define M_ZERO 0x02
#define M_CARRY 0x01

void initVM(struct VM *vm){
    memset(vm, 0, sizeof(struct VM));
    vm->sp = MEM_SIZE;
    vm->pc = 0;
    vm->sr = 0x10;
    vm->running = 1;
}

void printVM(struct VM *vm){
    printf("------Affichage------\n");

    printf("--REGISTERS--\n\n");
    for (size_t i = 0; i < N_REG; i++){
        printf("R%lu\t: %lx\n", i, vm->registers[i]);
    }
    printf("\nPC : %lx\nSP : %lx\nSR : %x\n\n", vm->pc, vm->sp, vm->sr);
    printf("--MEMORY--\n\n");

    for (size_t i = 0; i < MEM_SIZE; i++){
        printf("%lx\t: %x\n", i, vm->memory[i]);
    }
    printf("------Fin------\n\n");
}

void setSROF(struct VM *vm, uint8_t value){
    if (value){
        vm->sr |= M_OVERFLOW;
        return;
    }
    vm->sr &= ~M_OVERFLOW;
}

uint8_t getSROF(struct VM *vm){
    return vm->sr & M_OVERFLOW;
}

void setSRNF(struct VM *vm, uint8_t value){
    if (value){
        vm->sr |= M_NEGATIVE;
        return;
    }
    vm->sr &= ~M_NEGATIVE;
}

uint8_t getSRNF(struct VM *vm){
    return vm->sr & M_NEGATIVE;
}

void setSRZF(struct VM *vm, uint8_t value){
    if (value){
        vm->sr |= M_ZERO;
        return;
    }
    vm->sr &= ~M_ZERO;
}

uint8_t getSRZF(struct VM *vm){
    return vm->sr & M_ZERO;
}

void setSRCF(struct VM *vm, uint8_t value){
    if (value){
        vm->sr |= M_CARRY;
        return;
    }
    vm->sr &= ~M_CARRY;
}

uint8_t getSRCF(struct VM *vm){
    return vm->sr & M_CARRY;
}