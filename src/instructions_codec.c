#include "instructions_codec.h"
#include "memory.h"
#include "vm.h"
#include "instructions.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

int codecB(const struct Instruction *instr){
    return (instr->mode == REG) || (instr->mode == IND) || (instr->mode == IDX);
}

int codecARGS(const struct Instruction *instr){
    return (instr->mode == IMM) || (instr->mode == IDX) || (instr->mode == DIR);
}

size_t instructionEncode(const struct Instruction *instr, uint8_t *buffer, size_t remaining){
    if (instr->opcode >= OP_COUNT){
        fprintf(stderr, "OPCODE invalide\n");
        exit(EXIT_FAILURE);
    }

    size_t i = 0;
    if (remaining >= 2){
        buffer[i++] = (uint8_t) instr->opcode;
        buffer[i++] = (uint8_t) instr->mode;
        if (instr->mode == NONE){
            return i;
        }
        remaining -= 2;
    }
    else {
        return 0;
    }

    if (remaining >= 1){
        buffer[i++] = instr->a;
        remaining --;
    }
    else {
        return 0;
    }
    if (codecB(instr)){
        if (remaining >= 1){
            buffer[i++] = instr->b;
            remaining --;
        }
        else {
            return 0;
        }
    }

    if (codecARGS(instr)){
        if (remaining >= 8){
            for (size_t j = 0; j < 8; j++){
                buffer[i++] = (uint8_t) (((instr->mode == IMM) ? instr->args.imm : instr->args.offset) >> (j * 8));
            }
        }
        else {
            return 0;
        }
    }

    return i;
}

size_t instructionDecode(struct Instruction *instr, uint8_t *buffer, size_t remaining){

    if (remaining < 2){
        return 0;
    }
    size_t i = 0;
    instr->opcode = buffer[i++];
    if(instr->opcode >= OP_COUNT){
        fprintf(stderr, "OPCODE invalide\n");
        exit(EXIT_FAILURE);
    }

    instr->mode = buffer[i++];
    if (instr->mode == NONE){
        return i;
    }
    
    remaining -= 2;
    if (remaining < mode_size[instr->mode]){
        return 2;
    }

    instr->a = buffer[i++];
    if (codecB(instr)){
        instr->b = buffer[i++];
    }

    if (codecARGS(instr)){
        for (size_t j = 0; j < 8; j++){
            if (instr->mode == IDX){
                instr->args.offset |= ((uint64_t) buffer[i++]) << (j * 8);
            }
            else {
                instr->args.imm |= ((uint64_t) buffer[i++]) << (j * 8);
            }
        }
    }

    return i;
}