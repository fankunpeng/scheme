#ifndef _vm_h
#define _vm_h

#include <stdlib.h> // size_t, malloc, realloc

#include "value.h" // ptrvalue_t

struct vm_t {
    ptrvalue_t *head; // most recent allocated

    size_t allocated;
    size_t gc_threshold; // allocated needed to trigger gc
};

#endif // _vm_h