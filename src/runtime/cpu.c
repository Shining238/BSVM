#include "cpu.h"
#include "vm.h"
#include "instructions.h"
#include "instructions_codec.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

VM_Error cpu_step(struct VM *vm){

    if (vm->pc >= DATA_BASE){
        return INVALID_EXECUTION_ADDRESS;
    }
    struct Instruction instr;

    size_t consumed = instructionDecode(&instr, vm->memory + vm->pc, DATA_BASE - vm->pc - 1);
    if (consumed != mode_size[instr.mode]){
        return TRUNCATED_INSTR;
    }

    uint64_t old_pc = vm->pc;

    InstrHandler handler = get_handler(instr.opcode, instr.mode);
    if (handler == NULL){
        return UNKNOWN_INSTR;
    }

    VM_Error status = handler(vm, &instr);
    if (status == VM_OK && vm->pc == old_pc){ //dans le cas où l'execution n'est pas déroutée
        vm->pc += consumed;
    }

    return status;
}