#include "common.h"
#include "memory.h"
#include "vm.h"

#include <stdint.h>
#include <stdlib.h>


//lit 64 bits dans vm->memory à partir de addr
VM_Error memRead(struct VM *vm, uint64_t addr, int64_t *out){
    if (addr >= MEM_SIZE){
        return ILL_MEM_ACCESS;
    }
    
    for (size_t i = 0; i < 8; i++){
        *out |= (int64_t)vm->memory[addr++] << (8 * i);
    }
    return VM_OK;   
}

//écrit 64 bits dans vm->memory a partir de addr
VM_Error memWrite(struct VM *vm, uint64_t addr, int64_t value){
    if (addr >= MEM_SIZE){
        return ILL_MEM_ACCESS;
    }

    for (size_t i = 0; i < 8; i++){
        vm->memory[addr++] = (uint8_t)(value >> 8 * i);
    }
    return VM_OK;
}