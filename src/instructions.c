#include "instructions.h"
#include "memory.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

//ADD R0 5 (R0 += 5)

VM_Error instrADD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return INSTR_ILL;
    }
    vm->registers[instr->a] += (int64_t)instr->args.imm;
    return VM_OK;
}

//ADDR R0 R1 (R0 += R1)
VM_Error instrADDR(struct VM *vm, const struct Instruction *instr){
    if (instr->args.reg.b >= N_REG || instr->a >= N_REG){
        return INSTR_ILL;
    }
    vm->registers[instr->a] += vm->registers[instr->args.reg.b];
    return VM_OK;
}

//ADDD R0 [x] (R0 += MEM[x])
VM_Error instrADDD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return INSTR_ILL;
    }
    int64_t value;
    VM_Error status = memRead(vm, (uint64_t)instr->args.imm, &value);
    if (status == VM_OK){
        vm->registers[instr->a] = vm->registers[instr->a] + value;
    }
    return status;
}

//ADDI R0 [R1] (R0 += MEM[R1])
VM_Error instrADDI(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->args.reg.b >= N_REG){
        return INSTR_ILL;
    }

    int64_t value;
    VM_Error status = memRead(vm, (uint64_t)vm->registers[instr->args.reg.b], &value);
    if (status == VM_OK){
        vm->registers[instr->a] = vm->registers[instr->a] + value;
    }
    return status;
}

//ADDX R0 [R1+x] (R0 += MEM[R1+x])
VM_Error instrADDX(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->args.reg.b >= N_REG){
        return INSTR_ILL;
    }

    int64_t value;
    VM_Error status = memRead(vm, (uint64_t)(vm->registers[instr->args.reg.b] + (int64_t)instr->args.reg.offset), &value);
    if (status == VM_OK){
        vm->registers[instr->a] += value;
    }
    return status;
}

//SUB R0 5 (R0 -= 5)
VM_Error instrSUB(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return INSTR_ILL;
    }

    vm->registers[instr->a] -= (int64_t)instr->args.imm;
    return VM_OK;
}

//SUBR R0 R1 (R0 -= R1)
VM_Error instrSUBR(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->args.reg.b >= N_REG){
        return INSTR_ILL;
    }

    vm->registers[instr->a] -= vm->registers[instr->args.reg.b];
    return VM_OK;
}

//SUBD R0 [x] (R0 -= MEM[x])
VM_Error instrSUBD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return INSTR_ILL;
    }

    int64_t value;
    VM_Error status = memRead(vm, (uint64_t)instr->args.imm, &value);
    if (status == VM_OK){
        vm->registers[instr->a] -= value;
    }
    return status;
}

//SUBI R0 [R1] (R0 -= MEM[R1])
VM_Error instrSUBI(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->args.reg.b >= N_REG){
        return INSTR_ILL;
    }

    int64_t value;
    VM_Error status = memRead(vm, (uint64_t)instr->args.reg.b, &value);
    if (status == VM_OK){
        vm->registers[instr->a] -= value;
    }
    return status;
}

//SUBX R0 [R1+x] (R0 -= MEM[R1+x])
VM_Error instrSUBX(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->args.reg.b >= N_REG){
        return INSTR_ILL;
    }

    int64_t value;
    VM_Error status = memRead(vm, (uint64_t)(vm->registers[instr->args.reg.b] + (int64_t) instr->args.reg.offset), &value);
    if (status == VM_OK){
        vm->registers[instr->a] -= value;
    }
    return status;
}

//MUL R0 5 (R0 *= 5)
VM_Error instrMUL(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return INSTR_ILL;
    }

    vm->registers[instr->a] *= (int64_t)instr->args.imm;
    return VM_OK;
}

//MULR R0 R1 (R0 *= R1)
VM_Error instrMULR(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->args.reg.b >= N_REG){
        return INSTR_ILL;
    }

    vm->registers[instr->a] *= vm->registers[instr->args.reg.b];
    return VM_OK;
}

//MULD R0 [x] (R0 *= MEM[x])
VM_Error instrMULD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return INSTR_ILL;
    }

    int64_t value;
    VM_Error status = memRead(vm, (uint64_t)instr->args.imm, &value);
    if (status == VM_OK){
        vm->registers[instr->a] *= value;
    }
    return status;
}

//MULI R0 [R1] (R0 *= MEM[R1])
VM_Error instrMULI(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->args.reg.b >= N_REG){
        return INSTR_ILL;
    }

    int64_t value;
    VM_Error status = memRead(vm, (uint64_t) vm->registers[instr->args.reg.b], &value);
    if (status == VM_OK){
        vm->registers[instr->a] *= value;
    }
    return status;
}

//MULX R0 [R1+x] (R0 *= MEM[R1+x])
VM_Error instrMULX(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->args.reg.b >= N_REG){
        return INSTR_ILL;
    }

    int64_t value;
    VM_Error status = memRead(vm, (uint64_t)(vm->registers[instr->args.reg.b] + (int64_t)instr->args.reg.offset), &value);
    if (status == VM_OK){
        vm->registers[instr->a] *= value;
    }
    return status;
}


//DIV R0 5 (R0 = R0 / 5)
VM_Error instrDIV(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return INSTR_ILL;
    }
    if (instr->args.imm == 0){
        return DIV_0;
    }

    vm->registers[instr->a] /= (int64_t)instr->args.imm;
    return VM_OK;
}

//DIVR R0 R1 (R0 = R0 / R1)
VM_Error instrDIVR(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->args.reg.b >= N_REG){
        return INSTR_ILL;
    }
    if (vm->registers[instr->args.reg.b] == 0){
        return DIV_0;
    }

    vm->registers[instr->a] /= vm->registers[instr->args.reg.b];
    return VM_OK;
}

//DIVD R0 [x] (R0 /= MEM[x])
VM_Error instrDIVD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return INSTR_ILL;
    }

    int64_t value;
    VM_Error status = memRead(vm, (uint64_t) instr->args.imm, &value);
    if (status == VM_OK){
        if (value == 0){
            return DIV_0;
        }
        vm->registers[instr->a] /= value;
    }
    return status;
}

//DIVI R0 [R1] (R0 /= MEM[R1])
VM_Error instrDIVI(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->args.reg.b >= N_REG){
        return INSTR_ILL;
    }

    int64_t value;
    VM_Error status = memRead(vm, (uint64_t)vm->registers[instr->args.reg.b], &value);
    if (status == VM_OK){
        if (value == 0){
            return DIV_0;
        }
        vm->registers[instr->a] /= value;
    }
    return status;
}

//DIVX R0 [R1+x] (R0 /= MEM[R1+x])
VM_Error instrDIVX(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->args.reg.b >= N_REG){
        return INSTR_ILL;
    }

    int64_t value;
    VM_Error status = memRead(vm, (uint64_t) (vm->registers[instr->args.reg.b] + (int64_t)instr->args.reg.offset), &value);
    if (status == VM_OK){
        if (value == 0){
            return DIV_0;
        }
        vm->registers[instr->a] /= value;
    }
    return status;
}

//STD R0 [x] (MEM[x] = R0)
VM_Error instrSTD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return INSTR_ILL;
    }

    return memWrite(vm, (uint64_t) instr->args.imm, vm->registers[instr->a]);
}

//STI R0 R1 (MEM[R1] = R0)
VM_Error instrSTI(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->args.reg.b >= N_REG){
        return INSTR_ILL;
    }

    return memWrite(vm, (uint64_t)vm->registers[instr->args.reg.b], vm->registers[instr->a]);
}

//STX R0 [R1+x] (MEM[R1+x] = R0)
VM_Error instrSTX(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->args.reg.b >= N_REG){
        return INSTR_ILL;
    }

    return memWrite(vm, (uint64_t)(vm->registers[instr->args.reg.b] + (int64_t)instr->args.reg.offset), vm->registers[instr->a]);
}

//LD R0 x (R0 = x)
VM_Error instrLD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return INSTR_ILL;
    }

    vm->registers[instr->a] = (int64_t) instr->args.imm;
    return VM_OK;
}

//LDR R0 R1 (R0 = R1)
VM_Error instrLDR(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->args.reg.b >= N_REG){
        return INSTR_ILL;
    }

    vm->registers[instr->a] = vm->registers[instr->args.reg.b];
    return VM_OK;
}

//LDD R0 [x] (R0 = MEM[x])
VM_Error instrLDD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return INSTR_ILL;
    }

    return memRead(vm, (uint64_t) instr->args.imm, &vm->registers[instr->a]);
}

//LDI R0 [R1] (R0 = MEM[R1])
VM_Error instrLDI(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->args.reg.b >= N_REG){
        return INSTR_ILL;
    }

    return memRead(vm, (uint64_t)vm->registers[instr->args.reg.b], &vm->registers[instr->a]);
}

//LDX R0 [R1+x] (R0 = MEM[R1+x])
VM_Error instrLDX(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->args.reg.b >= N_REG){
        return INSTR_ILL;
    }

    return memRead(vm, (uint64_t)(vm->registers[instr->args.reg.b] + (int64_t) instr->args.reg.offset), &vm->registers[instr->a]);
}

//PUSH R0 (MEM[SP] = R0 & SP = SP-1)
VM_Error instrPUSH(struct VM *vm, const struct Instruction *instr){
    if (vm->sp == 0){
        return ILL_MEM_ACCESS;
    }
    if (instr->a >= N_REG){
        return INSTR_ILL;
    }

    vm->sp--;
    VM_Error write_status = memWrite(vm, vm->sp, vm->registers[instr->a]);
    if (write_status != VM_OK){
        vm->sp++;
    }
    
    return write_status;    
}

//POP R0 (R0 = MEM[SP] & SP = SP + 1)
VM_Error instrPOP(struct VM *vm, const struct Instruction *instr){
    if (vm->sp == MEM_SIZE){
        return ILL_MEM_ACCESS;
    }
    if (instr->a >= N_REG){
        return INSTR_ILL;
    }
    VM_Error read_status = memRead(vm, vm->sp, &vm->registers[instr->a]);
    if (read_status == VM_OK){
        vm->sp++;
    }

    return read_status;
}


static InstrHandler dispatch[OP_COUNT] = {
    [OP_ADD] = instrADD,
    [OP_ADDR] = instrADDR,
    [OP_ADDD] = instrADDD,
    [OP_ADDI] = instrADDI,
    [OP_ADDX] = instrADDX,
    [OP_SUB] = instrSUB,
    [OP_SUBR] = instrSUBR,
    [OP_SUBD] = instrSUBD,
    [OP_SUBI] = instrSUBI,
    [OP_SUBX] = instrSUBX,
    [OP_MUL] = instrMUL,
    [OP_MULR] = instrMULR,
    [OP_MULD] = instrMULD,
    [OP_MULI] = instrMULI,
    [OP_MULX] = instrMULX,
    [OP_DIV] = instrDIV,
    [OP_DIVR] = instrDIVR,
    [OP_DIVD] = instrDIVD,
    [OP_DIVI] = instrDIVI,
    [OP_DIVX] = instrDIVX,
    [OP_STD] = instrSTD,
    [OP_STI] = instrSTI,
    [OP_STX] = instrSTX,
    [OP_LD] = instrLD,
    [OP_LDR] = instrLDR,
    [OP_LDD] = instrLDD,
    [OP_LDI] = instrLDI,
    [OP_LDX] = instrLDX,
    [OP_PUSH] = instrPUSH,
    [OP_POP] = instrPOP,
};


InstrHandler get_handler(OP_CODE op_code){
    if (op_code >= OP_COUNT){
        return NULL;
    }
    return dispatch[op_code];
}