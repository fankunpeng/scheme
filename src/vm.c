#include <stdlib.h> // realloc
#include <string.h> // memset
#include <stdio.h>


#include "scheme.h"
#include "vm.h"
#include "value.h"

static void *scm_realloc(void *ptr, size_t size) {
    if (size == 0) {
        free(ptr);
        return NULL;
    }

    return realloc(ptr, size);
}

vm_t *vm_new() {
    vm_t *vm = (vm_t*) scm_realloc(NULL, sizeof(vm_t));

    memset(vm, 0, sizeof(vm_t));

    vm->allocated = 0;
    vm->gc_threshold = 8;
   
    vm->symbol_table = NULL;

    return vm;
}

void vm_free(vm_t *vm) {
    ptrvalue_t *ptr = vm->head;
    while (ptr != NULL) {
        ptrvalue_t *next = ptr->next;
        ptr_free(vm, ptr);
        ptr = next;
    }

    vm_realloc(vm, vm, 0, 0);
}


void *vm_realloc(vm_t *vm, void* ptr, size_t old_size, size_t new_size) {
    vm->allocated += new_size - old_size;

    if (new_size > 0 && vm->allocated > vm->gc_threshold) {
        gc(vm);
    }

    return scm_realloc(ptr, new_size);
}

// TODO: Implement GC
void gc(vm_t *vm) {
    return;
}
