#include "common.h"
#include "memory.h"
#include "vm.h"

#include <stdint.h>

VM_Error memRead(struct VM *vm, uint16_t addr, int32_t *out){
    if (addr > MEM_SIZE){
        return ILL_MEM_ACCESS;
    }
    
    *out = vm->memory[addr];
    return VM_OK;   
}

VM_Error memWrite(struct VM *vm, uint16_t addr, int32_t value){
    if (addr > MEM_SIZE){
        return ILL_MEM_ACCESS;
    }

    vm->memory[addr] = value;
    return VM_OK;
}