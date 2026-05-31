#pragma once
#include <stdint.h>
#include "vm.h"

VM_Error memRead(struct VM *vm, uint16_t addr, int32_t *out);

VM_Error memWrite(struct VM *vm, uint16_t addr, int32_t value);