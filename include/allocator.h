#pragma once
#include "common.h"
#include "vm.h"
#include <stdlib.h>

VM_Error vm_alloc(struct VM *vm, size_t size, uint64_t *addr);

VM_Error vm_free(struct VM *vm, uint64_t addr);