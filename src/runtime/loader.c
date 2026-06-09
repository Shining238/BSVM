#include "common.h"
#include "loader.h"
#include "vm.h"
#include "instructions.h"
#include "instructions_codec.h"
#include "memory.h"
#include "binary.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

size_t totalProgSize(const struct Instruction *program, size_t size){
    
    size_t total = 0;
    for (size_t i = 0; i < size; i++){
        total += mode_size[program[i].mode];
    }
    return total;
}

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

VM_Error loadProgram(struct VM *vm, struct Binary *bin, size_t start){
   
    struct BinaryHeader *headers = &bin->headers;
    if (headers->code_size + start >= DATA_BASE){
        return CODE_LOAD_OUT_OF_BOUNDS;
    }
    if (headers->data_size >= STACK_BASE - DATA_BASE){
        return DATA_LOAD_OUT_OF_BOUNDS;
    }

    memcpy(vm->memory + start, bin->bytecode, headers->code_size);
    memcpy(vm->memory + DATA_BASE, bin->data, headers->data_size);

    vm->pc = headers->entry_point;

    return VM_OK;
}