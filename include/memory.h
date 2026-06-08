#pragma once
#include <stdint.h>
#include "vm.h"

VM_Error memRead64(struct VM *vm, uint64_t addr, int64_t *out);

VM_Error memWrite64(struct VM *vm, uint64_t addr, int64_t value);

VM_Error memWriteByte(struct VM *vm, uint64_t addr, uint8_t byte);

VM_Error memReadByte(struct VM *vm, uint64_t addr, uint8_t *out);