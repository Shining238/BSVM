#include "common.h"
#include "loader.h"
#include "vm.h"
#include "instructions.h"
#include "instructions_codec.h"
#include "memory.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>


//buffer doit être de taille DATA_BASE
size_t encodeProgram(const struct Instruction *program, size_t prog_size, uint8_t *buffer){

    size_t cursor = 0;
    size_t written = 0;

    //on crée le bytecode
    for (size_t i = 0; i < prog_size; i++){
        written = instructionEncode(&program[i], buffer + cursor, (size_t) DATA_BASE - cursor - 1);
        if (written == 0){
            break;
        }
        cursor += written;
    }
    return cursor;
}

VM_Error loadProgram(struct VM *vm, uint8_t *bytecode, size_t size, uint64_t start){
    
    if (start + size >= DATA_BASE){
        return LOAD_OUT_OF_BOUNDS;
    }

    memcpy(vm->memory + start, bytecode, size);
    vm->pc = start;
    
    return VM_OK;
}