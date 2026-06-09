#include "instructions.h"
#include "memory.h"
#include "vm.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

void setSRADD(struct VM *vm, int64_t a, int64_t b, int64_t res){
    setSRZF(vm, 0);
    setSRNF(vm, 0);
    setSRCF(vm, 0);
    setSROF(vm, 0);
    if (res == 0){
        setSRZF(vm, 1);
    }
    if ((res >> 63) & 1){
        setSRNF(vm, 1);
    }
    if (res < a){
        setSRCF(vm, 1);
    }
    if ((~(a ^ b) & (a ^ res)) >> 63){
        setSROF(vm, 1);
    }
}

//ADD R0 5 (R0 += 5)
VM_Error instrADD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }
    int64_t a = vm->registers[instr->a];
    int64_t b = instr->args.imm;
    int64_t res = a + b;
    vm->registers[instr->a] = res;
   
    setSRADD(vm, a, b, res);
    return VM_OK;
}

//ADDR R0 R1 (R0 += R1)
VM_Error instrADDR(struct VM *vm, const struct Instruction *instr){
    if (instr->b >= N_REG || instr->a >= N_REG){
        return ILL_REGISTER;
    }
    int64_t a = vm->registers[instr->a];
    int64_t b = vm->registers[instr->b];
    int64_t res = a + b;
    setSRADD(vm, a, b, res);
    vm->registers[instr->a] = res;
    return VM_OK;
}

//ADDD R0 [x] (R0 += MEM[x])
VM_Error instrADDD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    if ((uint64_t)instr->args.imm < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    int64_t res;
    VM_Error status = memRead64(vm, (uint64_t)instr->args.imm, &value);
    if (status == VM_OK){
        res = vm->registers[instr->a] + value;
        setSRADD(vm, vm->registers[instr->a], value, res);
        vm->registers[instr->a] = res;
    }
    return status;
}

//ADDI R0 [R1] (R0 += MEM[R1])
VM_Error instrADDI(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = (uint64_t)vm->registers[instr->b];
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    int64_t res;
    VM_Error status = memRead64(vm, addr, &value);
    if (status == VM_OK){
        res = vm->registers[instr->a] + value;
        setSRADD(vm, vm->registers[instr->a], value, res);
        vm->registers[instr->a] = res;
    }
    return status;
}

//ADDX R0 [R1+x] (R0 += MEM[R1+x])
VM_Error instrADDX(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = (uint64_t)(vm->registers[instr->b] + instr->args.offset);
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    int64_t res;
    VM_Error status = memRead64(vm, addr, &value);
    if (status == VM_OK){
        res = vm->registers[instr->a] + value;
        setSRADD(vm, vm->registers[instr->a], value, res);
        vm->registers[instr->a] = res;
    }
    return status;
}

void setSRSUB(struct VM *vm, int64_t a, int64_t b, int64_t res){
    setSRZF(vm, 0);
    setSRNF(vm, 0);
    setSRCF(vm, 0);
    setSROF(vm, 0);
    if (res == 0){
        setSRZF(vm, 1);
    }
    if ((res >> 63) & 1){
        setSRNF(vm, 1);
    }
    if ((uint64_t) a < (uint64_t) b){
        setSRCF(vm, 1);
    }
    if (((a ^ b) & (a ^ res)) >> 63){
        setSROF(vm, 1);
    }
}

//SUB R0 5 (R0 -= 5)
VM_Error instrSUB(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }
    int64_t b = instr->args.imm;
    int64_t res = vm->registers[instr->a] - b;
    setSRSUB(vm, vm->registers[instr->a], b, res);

    vm->registers[instr->a] =res;
    return VM_OK;
}

//SUBR R0 R1 (R0 -= R1)
VM_Error instrSUBR(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }
    int64_t b = vm->registers[instr->b];
    int64_t res = vm->registers[instr->a] - b;
    setSRSUB(vm, vm->registers[instr->a], b, res);

    vm->registers[instr->a] = res;
    return VM_OK;
}

//SUBD R0 [x] (R0 -= MEM[x])
VM_Error instrSUBD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    if ((uint64_t) instr->args.imm < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    int64_t res;
    VM_Error status = memRead64(vm, (uint64_t)instr->args.imm, &value);
    if (status == VM_OK){
        res = vm->registers[instr->a] - value;
        setSRSUB(vm, vm->registers[instr->a], value, res);
        vm->registers[instr->a] = res;
    }
    return status;
}

//SUBI R0 [R1] (R0 -= MEM[R1])
VM_Error instrSUBI(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = vm->registers[instr->b];
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    int64_t res;
    VM_Error status = memRead64(vm, addr, &value);
    if (status == VM_OK){
        res = vm->registers[instr->a] - value;
        setSRSUB(vm, vm->registers[instr->a], value, res);
        vm->registers[instr->a] = res;
    }
    return status;
}

//SUBX R0 [R1+x] (R0 -= MEM[R1+x])
VM_Error instrSUBX(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = (uint64_t)(vm->registers[instr->b] + instr->args.offset);
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    int64_t res;
    VM_Error status = memRead64(vm, addr, &value);
    if (status == VM_OK){
        res = vm->registers[instr->a] - value;
        setSRSUB(vm, vm->registers[instr->a], value, res);
        vm->registers[instr->a] = res;
    }
    return status;
}

void setSRMUL(struct VM *vm, int64_t a, int64_t b, int64_t res){
    setSRZF(vm, 0);
    setSRNF(vm, 0);
    setSRCF(vm, 0);
    setSROF(vm, 0);
    __int128_t full = (__int128_t) a * (__int128_t) b;
    if (res == 0){
        setSRZF(vm, 1);
    }
    if ((res >> 63) & 1){
        setSRNF(vm, 1);
    }
    if (full > INT64_MAX || full < INT64_MIN){
        setSRCF(vm, 1);
        setSROF(vm, 1);
    }
}

//MUL R0 5 (R0 *= 5)
VM_Error instrMUL(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    int64_t b = instr->args.imm;
    int64_t res = vm->registers[instr->a] * b;
    setSRMUL(vm, vm->registers[instr->a], b, res);
    vm->registers[instr->a] = res;
    return VM_OK;
}

//MULR R0 R1 (R0 *= R1)
VM_Error instrMULR(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    int64_t b = vm->registers[instr->b];
    int64_t res = vm->registers[instr->a] * b;
    setSRMUL(vm, vm->registers[instr->a], b, res);
    vm->registers[instr->a] = res;
    return VM_OK;
}

//MULD R0 [x] (R0 *= MEM[x])
VM_Error instrMULD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    if ((uint64_t) instr->args.imm < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    int64_t res;
    VM_Error status = memRead64(vm, (uint64_t)instr->args.imm, &value);
    if (status == VM_OK){
        res = vm->registers[instr->a] * value;
        setSRMUL(vm, vm->registers[instr->a], value, res);
        vm->registers[instr->a] = res;
    }
    return status;
}

//MULI R0 [R1] (R0 *= MEM[R1])
VM_Error instrMULI(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = (uint64_t) vm->registers[instr->b];
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    int64_t res;
    VM_Error status = memRead64(vm, addr, &value);
    if (status == VM_OK){
        res = vm->registers[instr->a] * value;
        setSRMUL(vm, vm->registers[instr->a], value, res);
        vm->registers[instr->a] = res;
    }
    return status;
}

//MULX R0 [R1+x] (R0 *= MEM[R1+x])
VM_Error instrMULX(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = (uint64_t)(vm->registers[instr->b] + instr->args.offset);
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    int64_t res;
    VM_Error status = memRead64(vm, addr, &value);
    if (status == VM_OK){
        res = vm->registers[instr->a] * value;
        setSRMUL(vm, vm->registers[instr->a], value, res);
        vm->registers[instr->a] = res;
    }
    return status;
}

void setSRDIV(struct VM *vm, int64_t res){
    setSRZF(vm, 0);
    setSRNF(vm, 0);
    setSRCF(vm, 0);
    setSROF(vm, 0);
    if (res == 0){
        setSRZF(vm, 1);
    }
    if ((res >> 63) & 1){
        setSRNF(vm, 1);
    }
}

//DIV R0 5 (R0 = R0 / 5)
VM_Error instrDIV(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }
    if (instr->args.imm == 0){
        return DIV_0;
    }

    int64_t res = vm->registers[instr->a] / instr->args.imm;
    setSRDIV(vm, res);
    vm->registers[instr->a] = res;
    return VM_OK;
}

//DIVR R0 R1 (R0 = R0 / R1)
VM_Error instrDIVR(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }
    if (vm->registers[instr->b] == 0){
        return DIV_0;
    }

    int64_t res = vm->registers[instr->a] / vm->registers[instr->b];
    setSRDIV(vm, res);
    vm->registers[instr->a] = res;
    return VM_OK;
}

//DIVD R0 [x] (R0 /= MEM[x])
VM_Error instrDIVD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    if ((uint64_t) instr->args.imm < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    int64_t res;
    VM_Error status = memRead64(vm, (uint64_t) instr->args.imm, &value);
    if (status == VM_OK){
        if (value == 0){
            return DIV_0;
        }
        res = vm->registers[instr->a] / value;
        setSRDIV(vm, res);
        vm->registers[instr->a] = res;
    }
    return status;
}

//DIVI R0 [R1] (R0 /= MEM[R1])
VM_Error instrDIVI(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = (uint64_t)vm->registers[instr->b];
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    int64_t res;
    VM_Error status = memRead64(vm, addr, &value);
    if (status == VM_OK){
        if (value == 0){
            return DIV_0;
        }
        res = vm->registers[instr->a] / value;
        setSRDIV(vm, res);
        vm->registers[instr->a] = res;
    }
    return status;
}

//DIVX R0 [R1+x] (R0 /= MEM[R1+x])
VM_Error instrDIVX(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = (uint64_t) (vm->registers[instr->b] + instr->args.offset);
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    int64_t res;
    VM_Error status = memRead64(vm, addr, &value);
    if (status == VM_OK){
        if (value == 0){
            return DIV_0;
        }
        res = vm->registers[instr->a] / value;
        setSRDIV(vm, res);
        vm->registers[instr->a] = res;
    }
    return status;
}

//CMP R0 x (met les flags de SR suivant R0 - x)
VM_Error instrCMP(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    int64_t res = vm->registers[instr->a] - instr->args.imm;
    setSRSUB(vm, vm->registers[instr->a], instr->args.imm, res);
    return VM_OK;
}

//CMPR R0 R1 (met les flags de SR suivant R0 - R1)
VM_Error instrCMPR(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    int64_t res = vm->registers[instr->a] - vm->registers[instr->b];
    setSRSUB(vm, vm->registers[instr->a], vm->registers[instr->b], res);
    return VM_OK;
}

//CMPD R0 [x] (met les flags de SR suivant R0 - MEM[x])
VM_Error instrCMPD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }
    if ((uint64_t) instr->args.imm < DATA_BASE){
        return ILL_MEM_ACCESS;
    }

    int64_t value;
    int64_t res;
    VM_Error status = memRead64(vm, (uint64_t) instr->args.imm, &value);
    if (status == VM_OK){
        res = vm->registers[instr->a] - value;
        setSRSUB(vm, vm->registers[instr->a], value, res);
    }
    return status;
}

//CMPI R0 [R1] (met les flags de SR suivant R0 - MEM[R1])
VM_Error instrCMPI(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG ||instr->b >= N_REG){
        return ILL_REGISTER;
    }
    if ((uint64_t) vm->registers[instr->b] < DATA_BASE){
        return ILL_MEM_ACCESS;
    }

    int64_t value;
    int64_t res;
    VM_Error status = memRead64(vm, (uint64_t) vm->registers[instr->b], &value);
    if (status == VM_OK){
        res = vm->registers[instr->a] - value;
        setSRSUB(vm, vm->registers[instr->a], value, res);
    }
    return status;
}

//CMPX R0 [R1+x] (met les flags de SR suivant R0 - MEM[R1+x])
VM_Error instrCMPX(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG ||instr->b >= N_REG){
        return ILL_REGISTER;
    }
    uint64_t addr = (uint64_t) (vm->registers[instr->b] + instr->args.offset);
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }

    int64_t value;
    int64_t res;
    VM_Error status = memRead64(vm, addr, &value);
    if (status == VM_OK){
        res = vm->registers[instr->a] - value;
        setSRSUB(vm, vm->registers[instr->a], value, res);
    }
    return status;
}


void setSRNEG(struct VM *vm, int64_t a, int64_t res){
    setSRZF(vm, 0);
    setSRNF(vm, 0);
    setSRCF(vm, 0);
    setSROF(vm, 0);
    if (res == 0){
        setSRZF(vm, 1);
    }
    if ((res >> 63) & 1){
        setSRNF(vm, 1);
    }
    if (a == INT64_MIN){
        setSROF(vm, 1);
    }
    if (a != 0){
        setSRCF(vm, 1);
    }
}

//NEG R0 (R0 = -R0)
VM_Error instrNEG(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }
    setSRNEG(vm, vm->registers[instr->a], -vm->registers[instr->a]);
    vm->registers[instr->a] = -vm->registers[instr->a];
    return VM_OK;
}

void setSRLOG(struct VM *vm, int64_t res){
    setSRZF(vm, 0);
    setSRNF(vm, 0);
    setSRCF(vm, 0);
    setSROF(vm, 0);
    if (res == 0){
        setSRZF(vm, 1);
    }
    if ((res >> 63) & 1){
        setSRNF(vm, 1);
    }
}

//AND R0 x (R0 &= x)
VM_Error instrAND(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    vm->registers[instr->a] &= instr->args.imm;
    setSRLOG(vm, vm->registers[instr->a]);
    return VM_OK;
}

//ANDR R0 R1 (R0 &= R1)
VM_Error instrANDR(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    vm->registers[instr->a] &= vm->registers[instr->b];
    setSRLOG(vm, vm->registers[instr->a]);
    return VM_OK;
}

//ANDD R0 [x] (R0 &= MEM[x])
VM_Error instrANDD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    if ((uint64_t)instr->args.imm < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    VM_Error status = memRead64(vm, (uint64_t)instr->args.imm, &value);
    if (status == VM_OK){
        vm->registers[instr->a] &= value;
        setSRLOG(vm, vm->registers[instr->a]);
    }
    return status;
}

//ANDI R0 [R1] (R0 &= MEM[R1])
VM_Error instrANDI(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = (uint64_t)vm->registers[instr->b];
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    VM_Error status = memRead64(vm, addr, &value);
    if (status == VM_OK){
        vm->registers[instr->a] &= value;
        setSRLOG(vm, vm->registers[instr->a]);
    }
    return status;
}

//ANDX R0 [R1+x] (R0 &= MEM[R1+x])
VM_Error instrANDX(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = (uint64_t)(vm->registers[instr->b] + instr->args.offset);
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    VM_Error status = memRead64(vm, addr, &value);
    if (status == VM_OK){
        vm->registers[instr->a] &= value;
        setSRLOG(vm, vm->registers[instr->a]);
    }
    return status;
}

//OR R0 x (R0 |= x)
VM_Error instrOR(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    vm->registers[instr->a] |= instr->args.imm;
    setSRLOG(vm, vm->registers[instr->a]);
    return VM_OK;
}

//ORR R0 R1 (R0 |= R1)
VM_Error instrORR(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    vm->registers[instr->a] |= vm->registers[instr->b];
    setSRLOG(vm, vm->registers[instr->a]);
    return VM_OK;
}

//ORD R0 [x] (R0 |= MEM[x])
VM_Error instrORD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    if ((uint64_t)instr->args.imm < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    VM_Error status = memRead64(vm, (uint64_t)instr->args.imm, &value);
    if (status == VM_OK){
        vm->registers[instr->a] |= value;
        setSRLOG(vm, vm->registers[instr->a]);
    }
    return status;
}

//ORI R0 [R1] (R0 |= MEM[R1])
VM_Error instrORI(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = (uint64_t)vm->registers[instr->b];
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    VM_Error status = memRead64(vm, addr, &value);
    if (status == VM_OK){
        vm->registers[instr->a] |= value;
        setSRLOG(vm, vm->registers[instr->a]);
    }
    return status;
}

//ORX R0 [R1+x] (R0 |= MEM[R1+x])
VM_Error instrORX(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = (uint64_t)(vm->registers[instr->b] + instr->args.offset);
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    VM_Error status = memRead64(vm, addr, &value);
    if (status == VM_OK){
        vm->registers[instr->a] |= value;
        setSRLOG(vm, vm->registers[instr->a]);
    }
    return status;
}

//XOR R0 x (R0 ^= x)
VM_Error instrXOR(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    vm->registers[instr->a] ^= instr->args.imm;
    setSRLOG(vm, vm->registers[instr->a]);
    return VM_OK;
}

//XORR R0 R1 (R0 ^= R1)
VM_Error instrXORR(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    vm->registers[instr->a] ^= vm->registers[instr->b];
    setSRLOG(vm, vm->registers[instr->a]);
    return VM_OK;
}

//XORD R0 [x] (R0 ^= MEM[x])
VM_Error instrXORD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    if ((uint64_t)instr->args.imm < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    VM_Error status = memRead64(vm, (uint64_t)instr->args.imm, &value);
    if (status == VM_OK){
        vm->registers[instr->a] ^= value;
        setSRLOG(vm, vm->registers[instr->a]);
    }
    return status;
}

//XORI R0 [R1] (R0 ^= MEM[R1])
VM_Error instrXORI(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = (uint64_t)vm->registers[instr->b];
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    VM_Error status = memRead64(vm, addr, &value);
    if (status == VM_OK){
        vm->registers[instr->a] ^= value;
        setSRLOG(vm, vm->registers[instr->a]);
    }
    return status;
}

//XORX R0 [R1+x] (R0 ^= MEM[R1+x])
VM_Error instrXORX(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = (uint64_t)(vm->registers[instr->b] + instr->args.offset);
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    VM_Error status = memRead64(vm, addr, &value);
    if (status == VM_OK){
        vm->registers[instr->a] ^= value;
        setSRLOG(vm, vm->registers[instr->a]);
    }
    return status;
}
//NOT R0 (R0 = ~R0)
VM_Error instrNOT(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    vm->registers[instr->a] = ~vm->registers[instr->a];
    setSRLOG(vm, vm->registers[instr->a]);
    return VM_OK;
}


void setSRSHL(struct VM *vm, int64_t a, uint64_t n, int64_t res){
    setSRZF(vm, 0);
    setSRNF(vm, 0);
    setSRCF(vm, 0);
    setSROF(vm, 0);
    if (res == 0){
        setSRZF(vm, 1);
    }
    if ((res >> 63) & 1){
        setSRNF(vm, 1);
    }
    if ((a >> (63 - n + 1)) & 1){
        setSRCF(vm, 1);
    }
    if ((res >> 63) != (a >> 63)){
        setSROF(vm, 1);
    }
}


//SHL R0 x (R0 << x)
VM_Error instrSHL(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t n = (uint64_t) instr->args.imm;
    int64_t res = vm->registers[instr->a] << n;
    setSRSHL(vm, vm->registers[instr->a], n, res);
    vm->registers[instr->a] = res;
    return VM_OK;
}

//SHLR R0 R1 (R0 << R1)
VM_Error instrSHLR(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t n = (uint64_t) vm->registers[instr->b];
    int64_t res = vm->registers[instr->a] << n;
    setSRSHL(vm, vm->registers[instr->a], n, res);
    vm->registers[instr->a] = res;
    return VM_OK;
}

//SHLD R0 [x] (R0 << MEM[x])
VM_Error instrSHLD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    if ((uint64_t)instr->args.imm < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    int64_t res;
    VM_Error status = memRead64(vm, (uint64_t)instr->args.imm, &value);
    if (status == VM_OK){
        res = vm->registers[instr->a] << (uint64_t) value;
        setSRSHL(vm, vm->registers[instr->a], (uint64_t) value, res);
        vm->registers[instr->a] = res;
    }
    return status;
}

//SHLI R0 [R1] (R0 << MEM[R1])
VM_Error instrSHLI(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = (uint64_t)vm->registers[instr->b];
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    int64_t res;
    VM_Error status = memRead64(vm, addr, &value);
    if (status == VM_OK){
        res = vm->registers[instr->a] << (uint64_t) value;
        setSRSHL(vm, vm->registers[instr->a], (uint64_t) value, res);
        vm->registers[instr->a] = res;
    }
    return status;
}

//SHLX R0 [R1+x] (R0 << MEM[R1+x])
VM_Error instrSHLX(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = (uint64_t)(vm->registers[instr->b] + instr->args.offset);
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    int64_t res;
    VM_Error status = memRead64(vm, addr, &value);
    if (status == VM_OK){
        res = vm->registers[instr->a] << value;
        setSRSHL(vm, vm->registers[instr->a], (uint64_t) value, res);
        vm->registers[instr->a] = res;
    }
    return status;
}

void setSRSHR(struct VM *vm, int64_t a, uint64_t n, int64_t res){
    setSRZF(vm, 0);
    setSRNF(vm, 0);
    setSRCF(vm, 0);
    setSROF(vm, 0);
    if (res == 0){
        setSRZF(vm, 1);
    }
    if ((res >> 63) & 1){
        setSRNF(vm, 1);
    }
    if ((a >> (n - 1)) & 1){
        setSRCF(vm, 1);
    }
}


//SHR R0 x (R0 >> x)
VM_Error instrSHR(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    int64_t res = vm->registers[instr->a] >> (uint64_t) instr->args.imm;
    setSRSHR(vm, vm->registers[instr->a], (uint64_t) instr->args.imm, res);
    vm->registers[instr->a] = res;
    return VM_OK;
}

//SHRR R0 R1 (R0 >> R1)
VM_Error instrSHRR(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t n = (uint64_t) vm->registers[instr->b];
    int64_t res = vm->registers[instr->a] >> n;
    setSRSHR(vm, vm->registers[instr->a], n, res); 
    vm->registers[instr->a] = res;
    return VM_OK;
}

//SHRD R0 [x] (R0 >> MEM[x])
VM_Error instrSHRD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    if ((uint64_t) instr->args.imm < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    int64_t res;
    VM_Error status = memRead64(vm, (uint64_t)instr->args.imm, &value);
    if (status == VM_OK){
        res = vm->registers[instr->a] >> (uint64_t) value;
        setSRSHR(vm, vm->registers[instr->a], (uint64_t) value, res);
        vm->registers[instr->a] = res;
    }
    return status;
}

//SHRI R0 [R1] (R0 >> MEM[R1])
VM_Error instrSHRI(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = (uint64_t)vm->registers[instr->b];
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    int64_t res;
    VM_Error status = memRead64(vm, addr, &value);
    if (status == VM_OK){
        res = vm->registers[instr->a] >> (uint64_t) value;
        setSRSHR(vm, vm->registers[instr->a], (uint64_t) value, res);
        vm->registers[instr->a] = res;
    }
    return status;
}

//SHRX R0 [R1+x] (R0 >> MEM[R1+x])
VM_Error instrSHRX(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = (uint64_t)(vm->registers[instr->b] + instr->args.offset);
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    int64_t value;
    int64_t res;
    VM_Error status = memRead64(vm, addr, &value);
    if (status == VM_OK){
        res = vm->registers[instr->a] >> (uint64_t) value;
        setSRSHR(vm, vm->registers[instr->a], (uint64_t) value, res);
        vm->registers[instr->a] = res;
    }
    return status;
}

//STD R0 [x] (MEM[x] = R0)
VM_Error instrSTD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    if ((uint64_t) instr->args.imm < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    return memWrite64(vm, (uint64_t) instr->args.imm, vm->registers[instr->a]);
}

//STI R0 R1 (MEM[R1] = R0)
VM_Error instrSTI(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = (uint64_t)vm->registers[instr->b];
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    return memWrite64(vm, addr, vm->registers[instr->a]);
}

//STX R0 [R1+x] (MEM[R1+x] = R0)
VM_Error instrSTX(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = (uint64_t)(vm->registers[instr->b] + instr->args.offset);
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    return memWrite64(vm, addr, vm->registers[instr->a]);
}

//LD R0 x (R0 = x)
VM_Error instrLD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    vm->registers[instr->a] = instr->args.imm;
    return VM_OK;
}

//LDR R0 R1 (R0 = R1)
VM_Error instrLDR(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    vm->registers[instr->a] = vm->registers[instr->b];
    return VM_OK;
}

//LDD R0 [x] (R0 = MEM[x])
VM_Error instrLDD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    if ((uint64_t) instr->args.imm < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    return memRead64(vm, (uint64_t) instr->args.imm, &vm->registers[instr->a]);
}

//LDI R0 [R1] (R0 = MEM[R1])
VM_Error instrLDI(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }
    
    uint64_t addr = (uint64_t)vm->registers[instr->b];
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    return memRead64(vm, addr, &vm->registers[instr->a]);
}

//LDX R0 [R1+x] (R0 = MEM[R1+x])
VM_Error instrLDX(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }
    uint64_t addr = (uint64_t)(vm->registers[instr->b] + instr->args.offset);
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }

    return memRead64(vm, addr, &vm->registers[instr->a]);
}

//SWPR R0 R1 (R0 <-> R1)
VM_Error instrSWPR(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }
    int64_t tmp = vm->registers[instr->a];
    vm->registers[instr->a] = vm->registers[instr->b];
    vm->registers[instr->b] = tmp;
    return VM_OK;
}

//SWPD R0 [x] (R0 <-> MEM[x])
VM_Error instrSWPD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }
    uint64_t addr = (uint64_t) instr->args.imm;
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }

    int64_t tmp;
    VM_Error status = memRead64(vm, addr, &tmp);
    if (status == VM_OK){
        status = memWrite64(vm, addr, vm->registers[instr->a]);
        if (status == VM_OK){
            vm->registers[instr->a] = tmp;
        }
    }
    return status;
}

//SWPI R0 [R1] (R0 <-> MEM[R1])
VM_Error instrSWPI(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = (uint64_t) vm->registers[instr->b];
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }

    int64_t tmp;
    VM_Error status = memRead64(vm, addr, &tmp);
    if (status == VM_OK){
        status = memWrite64(vm, addr, vm->registers[instr->a]);
        if (status == VM_OK){
            vm->registers[instr->a] = tmp;
        }
    }
    return status;
}

//SWPX R0 [R1+x] (R0 <-> MEM[R1+x])
VM_Error instrSWPX(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG || instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = (uint64_t) (vm->registers[instr->b] + instr->args.offset);
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }

    int64_t tmp;
    VM_Error status = memRead64(vm, addr, &tmp);
    if (status == VM_OK){
        status = memWrite64(vm, addr, vm->registers[instr->a]);
        if (status == VM_OK){
            vm->registers[instr->a] = tmp;
        }
    }
    return status;
}

//PUSH R0 (MEM[SP] = R0 & SP = SP-1)
VM_Error instrPUSH(struct VM *vm, const struct Instruction *instr){
    if (vm->sp <= STACK_BASE){
        return STACK_OVERFLOW;
    }
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    vm->sp -= sizeof(vm->registers[instr->a]);
    VM_Error write_status = memWrite64(vm, vm->sp, vm->registers[instr->a]);
    if (write_status != VM_OK){
        vm->sp += sizeof(vm->registers[instr->a]);
    }
    
    return write_status;    
}

//POP R0 (R0 = MEM[SP] & SP = SP + 1)
VM_Error instrPOP(struct VM *vm, const struct Instruction *instr){
    if (vm->sp >= MEM_SIZE){
        return STACK_UNDERFLOW;
    }
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }
    VM_Error read_status = memRead64(vm, vm->sp, &vm->registers[instr->a]);
    if (read_status == VM_OK){
        vm->sp += sizeof(vm->registers[instr->a]);
    }

    return read_status;
}

//FASD R0 [x] (R0 = MEM[x] & MEM[x] = 1)
VM_Error instrFASD(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    if ((uint64_t) instr->args.imm < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    VM_Error status = memRead64(vm, (uint64_t)instr->args.imm, &vm->registers[instr->a]);
    if (status == VM_OK){
        status = memWrite64(vm, (uint64_t)instr->args.imm, 1);
    }
    return status;
} 

//FASI R0 [R1] (R0 = MEM[R1] & MEM[R1] = 1)
VM_Error instrFASI(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG && instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = (uint64_t)vm->registers[instr->b];
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    VM_Error status = memRead64(vm, addr, &vm->registers[instr->a]);
    if (status == VM_OK){
        status = memWrite64(vm, (uint64_t)instr->b, 1);
    }
    return status;
}

//FASX R0 [R1+x] (R0 = MEM[R1+x] & MEM[R1+x] = 1)
VM_Error instrFASX(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG && instr->b >= N_REG){
        return ILL_REGISTER;
    }

    uint64_t addr = (uint64_t)(vm->registers[instr->b] + instr->args.offset);
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }
    VM_Error status = memRead64(vm, addr, &vm->registers[instr->a]);
    if (status == VM_OK){
        status = memWrite64(vm, (uint64_t)(vm->registers[instr->b] + instr->args.offset), 1);
    }
    return status;
}

//CALL x (PUSH PC & JMP x)
VM_Error instrCALL(struct VM *vm, const struct Instruction *instr){

    if (vm->sp <= STACK_BASE){
        return STACK_OVERFLOW;
    }
    if ((uint64_t) instr->args.imm >= DATA_BASE){
        return INVALID_JMP;
    }

    vm->sp--;
     
    VM_Error status = memWrite64(vm, vm->sp, vm->pc);
    if (status == VM_OK){
        vm->pc = (uint64_t) instr->args.imm;
        return VM_OK;
    }
    vm->sp++;
    return status;
}

//CALLR R0 (PUSH PC & JMP [R0])
VM_Error instrCALLR(struct VM *vm, const struct Instruction *instr){
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }
    if (vm->sp <= STACK_BASE){
        return STACK_OVERFLOW;
    }
    if (vm->registers[instr->a] >= DATA_BASE){
        return INVALID_JMP;
    }

    vm->sp--;
     
    VM_Error status = memWrite64(vm, vm->sp, vm->pc);
    if (status == VM_OK){
        vm->pc = (uint64_t) vm->registers[instr->a];
        return VM_OK;
    }
    vm->sp++;
    return status;
}

//CALLD [x] (PUSH PC & JMP MEM[x])
VM_Error instrCALLD(struct VM *vm, const struct Instruction *instr){
    if (vm->sp <= STACK_BASE){
        return STACK_OVERFLOW;
    }
    
    int64_t addr;

    VM_Error status = memRead64(vm, (uint64_t) instr->args.imm, &addr);

    if (status != VM_OK){
        return status;
    }
    if (addr >= DATA_BASE){
        return INVALID_JMP;
    }
    vm->sp--;
     
    status = memWrite64(vm, vm->sp, vm->pc);
    if (status == VM_OK){
        vm->pc = (uint64_t)addr;
        return VM_OK;
    }
    vm->sp++;
    return status;
}

//CALLI [R0] (PUSH PC & JMP MEM[R0])
VM_Error instrCALLI(struct VM *vm, const struct Instruction *instr){
    if (vm->sp <= STACK_BASE){
        return STACK_OVERFLOW;
    }
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    int64_t addr;

    VM_Error status = memRead64(vm, (uint64_t)vm->registers[instr->a], &addr);
    if (status != VM_OK){
        return status;
    }
    if (addr >= DATA_BASE){
        return INVALID_JMP;
    }

    vm->sp--;
     
    status = memWrite64(vm, vm->sp, vm->pc);
    if (status == VM_OK){
        vm->pc = (uint64_t) addr;
        return VM_OK;
    }
    vm->sp++;
    return status;
}

//CALLX [R0+x] (PUSH PC & JMP MEM[R0+x])
VM_Error instrCALLX(struct VM *vm, const struct Instruction *instr){
    if (vm->sp <= STACK_BASE){
        return STACK_OVERFLOW;
    }
    if (instr->a >= N_REG){
        return ILL_REGISTER;
    }

    int64_t addr;

    VM_Error status = memRead64(vm, (uint64_t)(vm->registers[instr->a] + instr->args.offset), &addr);
    if (status != VM_OK){
        return status;
    }
    if (addr >= DATA_BASE){
        return INVALID_JMP;
    }

    vm->sp--;
     
    status = memWrite64(vm, vm->sp, vm->pc);
    if (status == VM_OK){
        vm->pc = (uint64_t) addr;
        return VM_OK;
    }
    vm->sp++;
    return status;
}

//RTN (POP PC)
VM_Error instrRTN(struct VM *vm, const struct Instruction *instr){
    if (vm->sp >= MEM_SIZE){
        return STACK_UNDERFLOW;
    }

    (void) instr;
    int64_t value;
    VM_Error status = memRead64(vm, vm->sp, &value);
    if (value >= DATA_BASE){
        return INVALID_JMP;
    }

    if (status == VM_OK){
        vm->pc = (uint64_t)value;
        vm->sp++;
    }
    return status;
}


//NOP (fait rien)
VM_Error instrNOP(struct VM *vm, const struct Instruction *instr){
    (void) vm;
    (void) instr;
    return VM_OK;
}

//RST (reinitialise la vm)
VM_Error instrRST(struct VM *vm, const struct Instruction *instr){
    (void) instr;
    initVM(vm);
    return VM_OK;
}

VM_Error jump(struct VM *vm, const struct Instruction *instr){
    if ((uint64_t) instr->args.imm >= DATA_BASE){
        return INVALID_JMP;
    }

    vm->pc = (uint64_t) instr->args.imm;
    return VM_OK;
}

VM_Error jumpr(struct VM *vm, const struct Instruction *instr){
    if (instr->a > N_REG){
        return ILL_REGISTER;
    }
    uint64_t addr = (uint64_t) vm->registers[instr->a];
    if (addr >= DATA_BASE){
        return INVALID_JMP;
    }

    vm->pc = addr;
    return VM_OK;
}

VM_Error jumpd(struct VM *vm, const struct Instruction *instr){
    if ((uint64_t) instr->args.imm >= DATA_BASE){
        return INVALID_JMP;
    }

    int64_t value;
    VM_Error status = memRead64(vm, (uint64_t) instr->args.imm, &value);
    if (status == VM_OK){
        if((uint64_t) value >= DATA_BASE){
            return INVALID_JMP;
        }
        vm->pc = (uint64_t) value;
    }
    return status;
}

VM_Error jumpi(struct VM *vm, const struct Instruction *instr){
    if (instr->a > N_REG){
        return ILL_REGISTER;
    }
    uint64_t addr = (uint64_t) vm->registers[instr->a];
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }

    int64_t value;
    VM_Error status = memRead64(vm, addr, &value);
    if (status == VM_OK){
        if ((uint64_t) value >= DATA_BASE){
            return INVALID_JMP;
        }
        vm->pc = (uint64_t) value;
    }
    return status;
}

VM_Error jumpx(struct VM *vm, const struct Instruction *instr){
    if (instr->a > N_REG){
        return ILL_REGISTER;
    }
    uint64_t addr = (uint64_t)(vm->registers[instr->a] + instr->args.offset);
    if (addr < DATA_BASE){
        return ILL_MEM_ACCESS;
    }

    int64_t value;
    VM_Error status = memRead64(vm, addr, &value);
    if (status == VM_OK){
        if ((uint64_t) value >= DATA_BASE){
            return INVALID_JMP;
        }
        vm->pc = (uint64_t) value;
    }
    return status;
}

//JMP x (PC = x)
VM_Error instrJMP(struct VM *vm, const struct Instruction *instr){
    return jump(vm, instr);
}

//JMPR R0 (PC = R0)
VM_Error instrJMPR(struct VM *vm, const struct Instruction *instr){
    return jumpr(vm, instr);
}

//JMPD [x] (PC = MEM[x])
VM_Error instrJMPD(struct VM *vm, const struct Instruction *instr){
    return jumpd(vm, instr);
}

//JMPI [R0] (PC = MEM[R0])
VM_Error instrJMPI(struct VM *vm, const struct Instruction *instr){
    return jumpi(vm, instr);
}

//JMPX [R0+x] (PC = MEM[R0+x])
VM_Error instrJMPX(struct VM *vm, const struct Instruction *instr){
    return jumpx(vm, instr);
}

//JEQ x (PC = x si Z dans SR)
VM_Error instrJEQ(struct VM *vm, const struct Instruction *instr){
    
    if (!getSRZF(vm)){
        return VM_OK;
    }

    return jump(vm, instr);
}

//JEQR R0 (PC = R0 si Z dans SR)
VM_Error instrJEQR(struct VM *vm, const struct Instruction *instr){
    if (!getSRZF(vm)){
        return VM_OK;
    }
    return jumpr(vm, instr);
}

//JEQD [x] (PC = MEM[x] si Z dans SR)
VM_Error instrJEQD(struct VM *vm, const struct Instruction *instr){
    if (!getSRZF(vm)){
        return VM_OK;
    }
    return jumpd(vm, instr);
}

//JEQI [R0] (PC = MEM[R0] si Z dans SR)
VM_Error instrJEQI(struct VM *vm, const struct Instruction *instr){
    if (!getSRZF(vm)){
        return VM_OK;
    }
    return jumpi(vm, instr);
}

//JEQX [R0+x] (PC = MEM[R0+x] si Z dans SR)
VM_Error instrJEQX(struct VM *vm, const struct Instruction *instr){
    if (!getSRZF(vm)){
        return VM_OK;
    }
   return jumpx(vm, instr);
}

//JNE x (PC = x si !Z dans SR)
VM_Error instrJNE(struct VM *vm, const struct Instruction *instr){
    
    if (getSRZF(vm)){
        return VM_OK;
    }
    return jump(vm, instr);
}

//JNER R0 (PC = R0 si !Z dans SR)
VM_Error instrJNER(struct VM *vm, const struct Instruction *instr){
    if (getSRZF(vm)){
        return VM_OK;
    }
    return jumpr(vm, instr);
}

//JNED [x] (PC = MEM[x] si !Z dans SR)
VM_Error instrJNED(struct VM *vm, const struct Instruction *instr){
    if (getSRZF(vm)){
        return VM_OK;
    }
    return jumpd(vm, instr);
}

//JNEI [R0] (PC = MEM[R0] si !Z dans SR)
VM_Error instrJNEI(struct VM *vm, const struct Instruction *instr){
    if (!getSRZF(vm)){
        return VM_OK;
    }
    return jumpi(vm, instr);
}

//JNEX [R0+x] (PC = MEM[R0+x] si !Z dans SR)
VM_Error instrJNEX(struct VM *vm, const struct Instruction *instr){
    if (getSRZF(vm)){
        return VM_OK;
    }
    return jumpx(vm, instr);
}

uint8_t eq_log(uint8_t a, uint8_t b){
    return ((a != 0) && (b != 0)) || ((a == 0) && (b == 0));
}

//JLE x (PC = x si Z || N != O dans SR)
VM_Error instrJLE(struct VM *vm, const struct Instruction *instr){
    if (!getSRZF(vm) && eq_log(getSRNF(vm), getSROF(vm))){
        return VM_OK;
    }
    return jump(vm, instr);
}

//JLER R0 (PC = R0 si Z || N != O  dans SR)
VM_Error instrJLER(struct VM *vm, const struct Instruction *instr){
    if (!getSRZF(vm) && eq_log(getSRNF(vm), getSROF(vm))){
        return VM_OK;
    }
    return jumpr(vm, instr);
}

//JLED [x] (PC = MEM[x] si Z || N != O dans SR)
VM_Error instrJLED(struct VM *vm, const struct Instruction *instr){
    if (!getSRZF(vm) && eq_log(getSRNF(vm), getSROF(vm))){
        return VM_OK;
    }
    return jumpd(vm, instr);
}

//JLEI [R0] (PC = MEM[R0] si Z || N != O dans SR)
VM_Error instrJLEI(struct VM *vm, const struct Instruction *instr){
    if (!getSRZF(vm) && eq_log(getSRNF(vm), getSROF(vm))){
        return VM_OK;
    }
    return jumpi(vm, instr);
}

//JLEX [R0+x] (PC = MEM[R0+x] si Z || N != O dans SR)
VM_Error instrJLEX(struct VM *vm, const struct Instruction *instr){
    if (!getSRZF(vm) && eq_log(getSRNF(vm), getSROF(vm))){
        return VM_OK;
    }
    return jumpx(vm, instr);
}

//JLT x (PC = x si N != O dans SR)
VM_Error instrJLT(struct VM *vm, const struct Instruction *instr){
    if (eq_log(getSRNF(vm), getSROF(vm))){
        return VM_OK;
    }
    return jump(vm, instr);
}

//JLTR R0 (PC = R0 si N != O  dans SR)
VM_Error instrJLTR(struct VM *vm, const struct Instruction *instr){
    if (eq_log(getSRNF(vm), getSROF(vm))){
        return VM_OK;
    }
    return jumpr(vm, instr);
}

//JLTD [x] (PC = MEM[x] N != O dans SR)
VM_Error instrJLTD(struct VM *vm, const struct Instruction *instr){
    if (eq_log(getSRNF(vm), getSROF(vm))){
        return VM_OK;
    }
    return jumpd(vm, instr);
}

//JLTI [R0] (PC = MEM[R0] si N != O dans SR)
VM_Error instrJLTI(struct VM *vm, const struct Instruction *instr){
    if (eq_log(getSRNF(vm), getSROF(vm))){
        return VM_OK;
    }
    return jumpi(vm, instr);
}

//JLTX [R0+x] (PC = MEM[R0+x] si N != O dans SR)
VM_Error instrJLTX(struct VM *vm, const struct Instruction *instr){
    if (eq_log(getSRNF(vm), getSROF(vm))){
        return VM_OK;
    }
    return jumpx(vm, instr);
}

//JGE x (PC = x si N = O dans SR)
VM_Error instrJGE(struct VM *vm, const struct Instruction *instr){
    if (!eq_log(getSRNF(vm), getSROF(vm))){
        return VM_OK;
    }
    return jump(vm, instr);
}

//JGER R0 (PC = R0 si N = O  dans SR)
VM_Error instrJGER(struct VM *vm, const struct Instruction *instr){
    if (!eq_log(getSRNF(vm), getSROF(vm))){
        return VM_OK;
    }
    return jumpr(vm, instr);
}

//JGED [x] (PC = MEM[x] N = O dans SR)
VM_Error instrJGED(struct VM *vm, const struct Instruction *instr){
    if (!eq_log(getSRNF(vm), getSROF(vm))){
        return VM_OK;
    }
    return jumpd(vm, instr);
}

//JGEI [R0] (PC = MEM[R0] si N = O dans SR)
VM_Error instrJGEI(struct VM *vm, const struct Instruction *instr){
    if (!eq_log(getSRNF(vm), getSROF(vm))){
        return VM_OK;
    }
    return jumpi(vm, instr);
}

//JGEX [R0+x] (PC = MEM[R0+x] si N = O dans SR)
VM_Error instrJGEX(struct VM *vm, const struct Instruction *instr){
    if (!eq_log(getSRNF(vm), getSROF(vm))){
        return VM_OK;
    }
    return jumpx(vm, instr);
}

//JGT x (PC = x si !Z && N = O dans SR)
VM_Error instrJGT(struct VM *vm, const struct Instruction *instr){
    if (getSRZF(vm) || !eq_log(getSRNF(vm), getSROF(vm))){
        return VM_OK;
    }
    return jump(vm, instr);
}

//JGTR R0 (PC = R0 si Z && !N = O  dans SR)
VM_Error instrJGTR(struct VM *vm, const struct Instruction *instr){
    if (getSRZF(vm) || !eq_log(getSRNF(vm), getSROF(vm))){
        return VM_OK;
    }
    return jumpr(vm, instr);
}

//JGTD [x] (PC = MEM[x] si !Z && N = O dans SR)
VM_Error instrJGTD(struct VM *vm, const struct Instruction *instr){
    if (getSRZF(vm) || !eq_log(getSRNF(vm), getSROF(vm))){
        return VM_OK;
    }
    return jumpd(vm, instr);
}

//JGTI [R0] (PC = MEM[R0] si !Z && N = O dans SR)
VM_Error instrJGTI(struct VM *vm, const struct Instruction *instr){
    if (getSRZF(vm) || !eq_log(getSRNF(vm), getSROF(vm))){
        return VM_OK;
    }
    return jumpi(vm, instr);
}

//JGTX [R0+x] (PC = MEM[R0+x] si !Z && N = O dans SR)
VM_Error instrJGTX(struct VM *vm, const struct Instruction *instr){
    if (getSRZF(vm) || !eq_log(getSRNF(vm), getSROF(vm))){
        return VM_OK;
    }
    return jumpx(vm, instr);
}

//HALT (stop l'execution du programme)
VM_Error instrHALT(struct VM *vm, const struct Instruction *instr){
    (void) instr;
    vm->running = 0;
    return VM_OK;
}

//static InstrHandler dispatch[OP_COUNT] = {
static InstrHandler dispatchADD[MODE_COUNT] = {
    [IMM] = instrADD,
    [REG] = instrADDR,
    [DIR] = instrADDD,
    [IND] = instrADDI,
    [IDX] = instrADDX
};

static InstrHandler dispatchSUB[MODE_COUNT] = {
    [IMM] = instrSUB,
    [REG] = instrSUBR,
    [DIR] = instrSUBD,
    [IND] = instrSUBI,
    [IDX] = instrSUBX
};

static InstrHandler dispatchMUL[MODE_COUNT] = {
    [IMM] = instrMUL,
    [REG] = instrMULR,
    [DIR] = instrMULD,
    [IND] = instrMULI,
    [IDX] = instrMULX
};

static InstrHandler dispatchDIV[MODE_COUNT] = {
    [IMM] = instrDIV,
    [REG] = instrDIVR,
    [DIR] = instrDIVD,
    [IND] = instrDIVI,
    [IDX] = instrDIVX
};

static InstrHandler dispatchCMP[MODE_COUNT] = {
    [IMM] = instrCMP,
    [REG] = instrCMPR,
    [DIR] = instrCMPD,
    [IND] = instrCMPI,
    [IDX] = instrCMPX
};

static InstrHandler dispatchNEG[MODE_COUNT] = {
    [REG] = instrNEG
};

static InstrHandler dispatchAND[MODE_COUNT] = {
    [IMM] = instrAND,
    [REG] = instrANDR,
    [DIR] = instrANDD,
    [IND] = instrANDI,
    [IDX] = instrANDX
};

static InstrHandler dispatchOR[MODE_COUNT] = {
    [IMM] = instrOR,
    [REG] = instrORR,
    [DIR] = instrORD,
    [IND] = instrORI,
    [IDX] = instrORX
};

static InstrHandler dispatchXOR[MODE_COUNT] = {
    [IMM] = instrXOR,
    [REG] = instrXORR,
    [DIR] = instrXORD,
    [IND] = instrXORI,
    [IDX] = instrXORX
};

static InstrHandler dispatchSHL[MODE_COUNT] = {
    [IMM] = instrSHL,
    [REG] = instrSHLR,
    [DIR] = instrSHLD,
    [IND] = instrSHLI,
    [IDX] = instrSHLX
};

static InstrHandler dispatchSHR[MODE_COUNT] = {
    [IMM] = instrSHR,
    [REG] = instrSHRR,
    [DIR] = instrSHRD,
    [IND] = instrSHRI,
    [IDX] = instrSHRX
};

static InstrHandler dispatchNOT[MODE_COUNT] = {
    [REG] = instrNOT
};

static InstrHandler dispatchST[MODE_COUNT] = {
    [DIR] = instrSTD,
    [IND] = instrSTI,
    [IDX] = instrSTX
};

static InstrHandler dispatchLD[MODE_COUNT] = {
    [IMM] = instrLD,
    [REG] = instrLDR,
    [DIR] = instrLDD,
    [IND] = instrLDI,
    [IDX] = instrLDX
};

static InstrHandler dispatchSWP[MODE_COUNT] = {
    [REG] = instrSWPR,
    [DIR] = instrSWPD,
    [IND] = instrSWPI,
    [IDX] = instrSWPX
};

static InstrHandler dispatchPUSH[MODE_COUNT] = {
    [REG] = instrPUSH
};

static InstrHandler dispatchPOP[MODE_COUNT] = {
    [REG] = instrPOP
};

static InstrHandler dispatchFAS[MODE_COUNT] = {
    [DIR] = instrFASD,
    [IND] = instrFASI,
    [IDX] = instrFASX
};

static InstrHandler dispatchCALL[MODE_COUNT] = {
    [IMM] = instrCALL,
    [REG] = instrCALLR,
    [DIR] = instrCALLD,
    [IND] = instrCALLI,
    [IDX] = instrCALLX
};

static InstrHandler dispatchRTN[MODE_COUNT] = {
    [NONE] = instrRTN
};

static InstrHandler dispatchNOP[MODE_COUNT] = {
    [NONE] = instrNOP
};

static InstrHandler dispatchJMP[MODE_COUNT] = {
    [IMM] = instrJMP,
    [REG] = instrJMPR,
    [DIR] = instrJMPD,
    [IND] = instrJMPI,
    [IDX] = instrJMPX
};

static InstrHandler dispatchJEQ[MODE_COUNT] = {
    [IMM] = instrJEQ,
    [REG] = instrJEQR,
    [DIR] = instrJEQD,
    [IND] = instrJEQI,
    [IDX] = instrJEQX
};

static InstrHandler dispatchJNE[MODE_COUNT] = {
    [IMM] = instrJNE,
    [REG] = instrJNER,
    [DIR] = instrJNED,
    [IND] = instrJNEI,
    [IDX] = instrJNEX
};

static InstrHandler dispatchJLE[MODE_COUNT] = {
    [IMM] = instrJLE,
    [REG] = instrJLER,
    [DIR] = instrJLED,
    [IND] = instrJLEI,
    [IDX] = instrJLEX
};

static InstrHandler dispatchJLT[MODE_COUNT] = {
    [IMM] = instrJLT,
    [REG] = instrJLTR,
    [DIR] = instrJLTD,
    [IND] = instrJLTI,
    [IDX] = instrJLTX
};

static InstrHandler dispatchJGE[MODE_COUNT] = {
    [IMM] = instrJGE,
    [REG] = instrJGER,
    [DIR] = instrJGED,
    [IND] = instrJGEI,
    [IDX] = instrJGEX
};

static InstrHandler dispatchJGT[MODE_COUNT] = {
    [IMM] = instrJGT,
    [REG] = instrJGTR,
    [DIR] = instrJGTD,
    [IND] = instrJGTI,
    [IDX] = instrJGTX
};

static InstrHandler dispatchHALT[MODE_COUNT] = {
    [NONE] = instrHALT
};

static InstrHandler dispatchRST[MODE_COUNT] = {
    [NONE] = instrRST
};

static InstrHandler *dispatch[OP_COUNT] = {
   [OP_ADD] = dispatchADD,
   [OP_SUB] = dispatchSUB,
   [OP_MUL] = dispatchMUL,
   [OP_DIV] = dispatchDIV,
   [OP_CMP] = dispatchCMP,
   [OP_NEG] = dispatchNEG,
   [OP_AND] = dispatchAND,
   [OP_OR] = dispatchOR,
   [OP_XOR] = dispatchXOR,
   [OP_NOT] = dispatchNOT,
   [OP_SHL] = dispatchSHL,
   [OP_SHR] = dispatchSHR,
   [OP_ST] = dispatchST,
   [OP_LD] = dispatchLD,
   [OP_SWP] = dispatchSWP,
   [OP_PUSH] = dispatchPUSH,
   [OP_POP] = dispatchPOP,
   [OP_FAS] = dispatchFAS,
   [OP_CALL] = dispatchCALL,
   [OP_RTN] = dispatchRTN,
   [OP_NOP] = dispatchNOP,
   [OP_RST] = dispatchRST,
   [OP_JMP] = dispatchJMP,
   [OP_JEQ] = dispatchJEQ,
   [OP_JNE] = dispatchJEQ,
   [OP_JLE] = dispatchJLE,
   [OP_JLT] = dispatchJLT,
   [OP_JGE] = dispatchJGE,
   [OP_JGT] = dispatchJGT,
   [OP_HALT] = dispatchHALT
};



InstrHandler get_handler(OP_CODE op_code, MODE mode){
    if (op_code >= OP_COUNT){
        return NULL;
    }
    if (mode >= MODE_COUNT){
        return NULL;
    }
    return dispatch[op_code][mode];
}