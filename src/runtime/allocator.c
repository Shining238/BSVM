#include "common.h"
#include "allocator.h"
#include "vm.h"

#include <stdlib.h>
#include <stdio.h>


VM_Error vm_alloc(struct VM *vm, size_t size, uint64_t *addr){

    struct MemNode *node = vm->memblocks;
    while (node && ((node->size < size) || !node->free))  node = node->next;
    if (node == NULL){
        return VM_HEAP_OUT_OF_MEMORY;
    }
    *addr = node->begin;

    struct MemNode *suiv = node->next;
    
    node->next = malloc(sizeof(struct MemNode));
    node->next->begin = node->begin + size;
    node->next->size = node->size - size;
    node->next->next = suiv;
    node->next->free = 1;

    node->size = size;
    node->free = 0;
    return VM_OK;
}

VM_Error vm_free(struct VM *vm, uint64_t addr){

    if ((addr < HEAP_BASE) || (addr >= STACK_BASE)) return VM_FREE_OUT_OF_THE_HEAP;

    struct MemNode *node = vm->memblocks;

    while (node && (addr != node->begin)) node = node->next;
    if (!node) return VM_MEM_ALREADY_FREE;

    node->free = 1;
    struct MemNode *suiv;
    if (node->next && node->next->free){
        node->size += node->next->size;
        suiv = node->next->next;
        free(node->next);
        node->next = suiv;
    }
    return VM_OK;
}
