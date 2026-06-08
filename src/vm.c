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

char *errorToString(VM_Error status){
    char *str = malloc(64);
    switch (status){
        case VM_OK: memcpy(str, "VM OK", 6); break;
        case DIV_0: memcpy(str, "Division par zero", 18); break;
        case INSTR_PRIV: memcpy(str, "Instruction privilégiée", 26); break;
        case INSTR_TRAP: memcpy(str, "Instruction TRAP", 17); break;
        case ILL_MEM_ACCESS: memcpy(str, "Accès mémoire illégal", 25); break;
        case ILL_REGISTER: memcpy(str, "Registre invalide", 18); break;
        case STACK_UNDERFLOW: memcpy(str, "Stack Underflow", 16); break;
        case STACK_OVERFLOW: memcpy(str, "Stack Overflow", 15); break;
        case INVALID_JMP: memcpy(str, "Jump hors du segment de code", 29); break;
        case LOAD_OUT_OF_BOUNDS: memcpy(str, "Espace du segment code insuffisant", 35); break;
        case INVALID_EXECUTION_ADDRESS: memcpy(str, "PC pointe hors du segment de code", 34); break;
        case TRUNCATED_INSTR: memcpy(str, "Instruction tronquée", 22); break;
        case UNKNOWN_INSTR: memcpy(str, "Instruction indéfinie", 23); break;
        default: memcpy(str, "Something went really wrong :(", 31); break;
    }
    return str;
}

void printVM(struct VM *vm){
    printf("------Affichage------\n");

    printf("--Etat--\n\n");
    printf("%s\n\n", (vm->running ? "Running !" : "Halted !"));
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
