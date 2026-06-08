#pragma once

#include "common.h"
#include "vm.h"
#include "instructions.h"
#include <stdint.h>
#include <stdlib.h>

size_t instruction_encode(const struct Instruction *instr, uint8_t *buffer);

size_t instruction_decode(struct Instruction *instr, uint8_t *buffer);