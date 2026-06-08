#pragma once

#include "vm.h"
#include "instructions.h"

size_t totalProgSize(const struct Instruction *program, size_t size);

size_t encodeProgram(const struct Instruction *program, size_t prog_size, uint8_t *buffer);

VM_Error loadProgram(struct VM *vm, uint8_t *bytecode, size_t size, uint64_t start);