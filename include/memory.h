#pragma once
#include <stdint.h>
#include "vm.h"

VM_Error memRead(struct VM *vm, uint64_t addr, int64_t *out);

VM_Error memWrite(struct VM *vm, uint64_t addr, int64_t value);