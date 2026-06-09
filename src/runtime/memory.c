#include "common.h"
#include "memory.h"
#include "vm.h"

#include <stdint.h>
#include <stdlib.h>


//lit 64 bits dans vm->memory à partir de addr et le place dans *out
VM_Error memRead64(struct VM *vm, uint64_t addr, int64_t *out){
    if (addr >= MEM_SIZE){
        return VM_ILL_MEM_ACCESS;
    }
    
    for (size_t i = 0; i < 8; i++){
        *out |= (int64_t)vm->memory[addr++] << (8 * i);
    }
    return VM_OK;   
}

//écrit 64 bits dans vm->memory a partir de addr
VM_Error memWrite64(struct VM *vm, uint64_t addr, int64_t value){
    if (addr >= MEM_SIZE){
        return VM_ILL_MEM_ACCESS;
    }

    for (size_t i = 0; i < 8; i++){
        vm->memory[addr++] = (uint8_t)(value >> 8 * i);
    }
    return VM_OK;
}

//écrit un octet dans vm->memory à addr
VM_Error memWriteByte(struct VM *vm, uint64_t addr, uint8_t byte){
    if (addr >= MEM_SIZE){
        return VM_ILL_MEM_ACCESS;
    }

    vm->memory[addr] = byte;
    return VM_OK;
}

//lit un octer dans vm->memory à addr et le place dans *out
VM_Error memReadByte(struct VM *vm, uint64_t addr, uint8_t *out){
    if (addr >= MEM_SIZE){
        return VM_ILL_MEM_ACCESS;
    }

    *out = vm->memory[addr];
    return VM_OK;
}
