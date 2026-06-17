#include "common.h"
#include "vm.h"
#include "cpu.h"
#include "allocator.h"

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

    vm->memblocks = malloc(sizeof(*(vm->memblocks)));
    vm->memblocks->begin = HEAP_BASE;
    vm->memblocks->free = 1;
    vm->memblocks->next = NULL;
    vm->memblocks->size = STACK_BASE - HEAP_BASE;
}

void destroyVM(struct VM *vm){
    struct MemNode *node = vm->memblocks;
    struct MemNode *suiv;
    while (node){
        suiv = node->next;
        free(node);
        node = suiv;
    }
}

VM_Error vm_run(struct VM *vm){
    VM_Error status;
    size_t cycle = 0;
    while (((status = cpu_step(vm)) == VM_OK) && vm->running) cycle++;
    if (status != VM_OK) printf("Nb de cycles : %lu\n", cycle+1);
    return status;
}

char *errorToString(VM_Error status){
    char *str = malloc(64);
    switch (status){
        case VM_OK: memcpy(str, "VM OK", 6); break;
        case VM_DIV_ZERO: memcpy(str, "Division par zero", 18); break;
        case VM_INSTR_PRIV: memcpy(str, "Instruction privilégiée", 26); break;
        case VM_INSTR_TRAP: memcpy(str, "Instruction TRAP", 17); break;
        case VM_ILL_MEM_ACCESS: memcpy(str, "Accès mémoire illégal", 25); break;
        case VM_ILL_REGISTER: memcpy(str, "Registre invalide", 18); break;
        case VM_STACK_UNDERFLOW: memcpy(str, "Stack Underflow", 16); break;
        case VM_STACK_OVERFLOW: memcpy(str, "Stack Overflow", 15); break;
        case VM_INVALID_JMP: memcpy(str, "Jump hors du segment de code", 29); break;
        case VM_CODE_LOAD_OUT_OF_BOUNDS: memcpy(str, "Espace du segment code insuffisant", 35); break;
        case VM_DATA_LOAD_OUT_OF_BOUNDS: memcpy(str, "Espace du segment data insuffisant", 35); break;
        case VM_INVALID_EXECUTION_ADDRESS: memcpy(str, "PC pointe hors du segment de code", 34); break;
        case VM_TRUNCATED_INSTR: memcpy(str, "Instruction tronquée", 22); break;
        case VM_UNKNOWN_INSTR: memcpy(str, "Instruction indéfinie", 23); break;
        case VM_UNKNOWN_SYSCALL: memcpy(str, "Syscall indéfini", 18); break;
        case VM_HEAP_OUT_OF_MEMORY: memcpy(str, "Heap is out of memory", 22); break;
        case VM_MEM_ALREADY_FREE: memcpy(str, "Memory if already free", 23); break;
        case VM_OUT_OF_THE_HEAP: memcpy(str, "Address points out of the heap", 31); break;
        case VM_FREE_OUT_OF_THE_HEAP: memcpy(str, "Tried to free memory out of the heap", 37); break;
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
