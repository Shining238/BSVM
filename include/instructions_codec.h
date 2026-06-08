#pragma once

#include "common.h"
#include "vm.h"
#include "instructions.h"
#include <stdint.h>
#include <stdlib.h>

size_t instructionEncode(const struct Instruction *instr, uint8_t *buffer, size_t remaining);

size_t instructionDecode(struct Instruction *instr, uint8_t *buffer, size_t remaining);