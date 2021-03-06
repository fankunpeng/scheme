#ifndef _value_h
#define _value_h

#include <math.h>     // trunc
#include <stdbool.h>  // bool
#include <stdint.h>   // uint64_t, uintptr_t

#include "config.h"
#include "scheme.h"

typedef enum {
    T_CONS,
    T_STRING,
    T_SYMBOL,
    T_PRIMITIVE,
    T_FUNCTION,
    T_MACRO,
    T_VECTOR,
    T_ENV
} ptrvalue_type_t;

// ptrvalue is a heap allocated object
typedef struct _ptrvalue {
    ptrvalue_type_t type;
    bool gcmark;
    // next heap allocated object
    struct _ptrvalue *next;
} ptrvalue_t;

#if NANTAG

// a generic value
typedef uint64_t value_t;

#else  // !NANTAG

typedef enum {
    V_NIL,
    V_TRUE,
    V_FALSE,
    V_UNDEFINED,
    V_VOID,
    V_EOF,
    V_NUM,
    V_PTR
} value_type_t;

typedef struct {
    value_type_t type;
    union {
        double num;
        ptrvalue_t *ptr;
    } v;
} value_t;

#endif

// a cons cell
typedef struct {
    ptrvalue_t p;

    value_t car, cdr;
} cons_t;

// a basic string type
typedef struct {
    ptrvalue_t p;

    uint32_t len, hash;
    // C99 only - flexible array
    char value[];
} string_t;

// a basic symbol type with a pointer
// to the next one in the symbol_table
typedef struct _symbol_t {
    ptrvalue_t p;

    uint32_t len;

    struct _symbol_t *next;

    char name[];
} symbol_t;

// Environment frame
// Should be typedef'd by now
struct _env_t {
    ptrvalue_t p;

    // contains variables and their mapping in assoc list
    // ((var . val) (var2 . val2) ... )
    // or NIL_VAL
    value_t variables;

    // points to 'upper' env, NULL if none
    struct _env_t *up;
};

// A primitive (builtin) function C type
typedef value_t (*primitive_fn)(vm_t *vm, env_t *env, value_t args);

typedef struct {
    ptrvalue_t p;

    symbol_t *name;
    primitive_fn fn;
} primitive_t;

// User-defined function or a macro
typedef struct {
    ptrvalue_t p;

    symbol_t *name;
    env_t *env;

    value_t params;
    value_t body;
} function_t;

// A dynamic array (vector) type
typedef struct {
    ptrvalue_t p;

    uint32_t capacity, count;
    value_t *data;
} vector_t;

// C value -> value
#define BOOL_VAL(b) ((b) ? TRUE_VAL : FALSE_VAL)
#define NUM_VAL(num) (num_to_val(num))
#define PTR_VAL(ptr) (ptr_to_val((ptrvalue_t *) (ptr)))

#define IS_BOOL(val) (IS_FALSE(val) || IS_TRUE(val))

#define IS_INT(val) (IS_NUM(val) && (trunc(AS_NUM(val)) == AS_NUM(val)))
#define IS_DOUBLE(val) (IS_NUM(val) && !(trunc(AS_NUM(val)) == AS_NUM(val)))

#define IS_CONS(val) (val_is_ptr(val, T_CONS))
#define IS_STRING(val) (val_is_ptr(val, T_STRING))
#define IS_SYMBOL(val) (val_is_ptr(val, T_SYMBOL))
#define IS_PRIMITIVE(val) (val_is_ptr(val, T_PRIMITIVE))
#define IS_FUNCTION(val) (val_is_ptr(val, T_FUNCTION))
#define IS_MACRO(val) (val_is_ptr(val, T_MACRO))
#define IS_VECTOR(val) (val_is_ptr(val, T_VECTOR))
#define IS_ENV(val) (val_is_ptr(val, T_ENV))

#define IS_PROCEDURE(val) (IS_PRIMITIVE(val) || IS_FUNCTION(val))

// doesn't check anything
#define AS_CONS(val) ((cons_t *) AS_PTR(val))
#define AS_STRING(val) ((string_t *) AS_PTR(val))
#define AS_SYMBOL(val) ((symbol_t *) AS_PTR(val))
#define AS_PRIMITIVE(val) ((primitive_t *) AS_PTR(val))
#define AS_FUNCTION(val) ((function_t *) AS_PTR(val))
#define AS_MACRO(val) (AS_FUNCTION(val))
#define AS_VECTOR(val) ((vector_t *) AS_PTR(val))
#define AS_ENV(val) ((env_t *) AS_PTR(val))

#define AS_NUM(val) (val_to_num(val))
#define AS_INT(val) ((int64_t) trunc(val_to_num(val)))

#define MAKE_STRING(vm, s) (string_new((vm), (s), sizeof(s) - 1))

#if NANTAG

// 1---------------------------------------------------------------
#define SIGN_BIT ((uint64_t) 1 << 63)
// -1111111111111--------------------------------------------------
#define QUIET_NAN ((uint64_t) 0x7ffc000000000000)

// used for singletons
// -------------------------------------------------------------111
#define MASK_TAG (7)

#define GET_TAG(val) ((int) ((val) & (MASK_TAG)))

// NaN is reserved!
#define TAG_NAN (0)

#define TAG_NIL (1)
#define TAG_TRUE (2)
#define TAG_FALSE (3)
#define TAG_UNDEFINED (4)
#define TAG_VOID (5)
#define TAG_EOF (6)
#define TAG_UNUSED (7)  // this is unused !!!!

#define NIL_VAL ((value_t)(uint64_t)(QUIET_NAN | TAG_NIL))
#define TRUE_VAL ((value_t)(uint64_t)(QUIET_NAN | TAG_TRUE))
#define FALSE_VAL ((value_t)(uint64_t)(QUIET_NAN | TAG_FALSE))
#define UNDEFINED_VAL ((value_t)(uint64_t)(QUIET_NAN | TAG_UNDEFINED))
#define VOID_VAL ((value_t)(uint64_t)(QUIET_NAN | TAG_VOID))
#define EOF_VAL ((value_t)(uint64_t)(QUIET_NAN | TAG_EOF))

#define IS_NIL(val) ((val) == NIL_VAL)
#define IS_TRUE(val) ((val) == TRUE_VAL)
#define IS_FALSE(val) ((val) == FALSE_VAL)
#define IS_UNDEFINED(val) ((val) == UNDEFINED_VAL)
#define IS_VOID(val) ((val) == VOID_VAL)
#define IS_EOF(val) ((val) == EOF_VAL)

// returns true if val is type <X> in IS_<X>
#define IS_NUM(val) (((val) & (QUIET_NAN)) != QUIET_NAN)
#define IS_PTR(val) (((val) & (QUIET_NAN | SIGN_BIT)) == (QUIET_NAN | SIGN_BIT))
// if a value is not ptrvalue
#define IS_VAL(val) (!IS_PTR(val))

// value -> C value
// all values but #f and NIL are truthy!
#define AS_BOOL(val) ((!IS_FALSE(val)) && (!IS_NIL(val)))
#define AS_PTR(val) \
    ((ptrvalue_t *) (uintptr_t)((val) & ~(SIGN_BIT | QUIET_NAN)))

#else  // !NANTAG

#define AS_BOOL(val) (((val).type != V_FALSE) && ((val).type != V_NIL))

#define AS_PTR(val) ((val).v.ptr)

#define IS_PTR(val) ((val).type == V_PTR)
#define IS_VAL(val) ((val).type != V_PTR)

#define IS_NIL(val) ((val).type == V_NIL)
#define IS_TRUE(val) ((val).type == V_TRUE)
#define IS_FALSE(val) ((val).type == V_FALSE)
#define IS_UNDEFINED(val) ((val).type == V_UNDEFINED)
#define IS_VOID(val) ((val).type == V_VOID)
#define IS_EOF(val) ((val).type == V_EOF)

#define IS_NUM(val) ((val).type == V_NUM)

#define NIL_VAL ((value_t){V_NIL, {0}})
#define TRUE_VAL ((value_t){V_TRUE, {0}})
#define FALSE_VAL ((value_t){V_FALSE, {0}})
#define UNDEFINED_VAL ((value_t){V_UNDEFINED, {0}})
#define VOID_VAL ((value_t){V_VOID, {0}})
#define EOF_VAL ((value_t){V_EOF, {0}})

#endif  // NANTAG

// equality
#define IS_EQ(a, b) (val_eq((a), (b)))

/* *** Memory management functions *** */

void ptr_free(vm_t *vm, ptrvalue_t *ptr);

cons_t *cons_new(vm_t *vm);
string_t *string_new(vm_t *vm, const char *text, size_t len);
symbol_t *symbol_new(vm_t *vm, const char *name, size_t len);
primitive_t *primitive_new(vm_t *vm, primitive_fn fn);
function_t *function_new(vm_t *vm, env_t *env, value_t params, value_t body);
function_t *macro_new(vm_t *vm, env_t *env, value_t params, value_t body);
vector_t *vector_new(vm_t *vm, uint32_t count);
env_t *env_new(vm_t *vm, value_t variables, env_t *up);

// Makes sure that there are no duplicit symbols
// => we can compare symbols using pointer comparisons
symbol_t *symbol_intern(vm_t *vm, const char *name, size_t len);

// Creates a new cons cell and puts a as car and b as cdr
value_t cons_fn(vm_t *vm, value_t a, value_t b);

// Gets the length of a cons cell
int32_t cons_len(value_t val);

// Pushes <val> to the back of <vec>
// (equivalent to std::vector.push_back(val))
void vector_push(vm_t *vm, vector_t *vec, value_t val);

/* *** conversion utilities *** */

// a conversion type from double to uint64_t
typedef union {
    uint64_t bits;
    double num;
} value_conv_t;

static inline double val_to_num(value_t val) {
#if NANTAG
    value_conv_t data;
    data.bits = val;
    return data.num;
#else   // !NANTAG
    return val.v.num;
#endif  // NANTAG
}

static inline value_t num_to_val(double num) {
#if NANTAG
    value_conv_t data;
    data.num = num;
    return data.bits;
#else   // !NANTAG
    value_t val;
    val.type = V_NUM;
    val.v.num = num;
    return val;
#endif  // NANTAG
}

static inline value_t ptr_to_val(ptrvalue_t *ptr) {
#if NANTAG
    return (value_t)(SIGN_BIT | QUIET_NAN | (uint64_t)(uintptr_t)(ptr));
#else  // !NANTAG
    value_t val;
    val.type = V_PTR;
    val.v.ptr = ptr;
    return val;
#endif
}

/* *** equality *** */

static inline bool val_is_ptr(value_t val, ptrvalue_type_t t) {
    return IS_PTR(val) && AS_PTR(val)->type == t;
}

// scm: equal?
// true for both ptrvalues and values when they are equal by value
bool val_equal(value_t a, value_t b);

// scm: eq? or eqv?
// true for ptrvalues when they are identical
// true for values when they are equal
static inline bool val_eq(value_t a, value_t b) {
#if NANTAG
    return a == b;
#else   // !NANTAG
    if (a.type != b.type) return false;
    if (a.type == V_NUM) return (a.v.num == b.v.num);
    return a.v.ptr == b.v.ptr;
#endif  // NANTAG
}

/* *** hashing *** */

// hashes a value
// (careful, has to be immutable [IS_VAL || IS_STRING])
uint32_t hash_value(value_t val);

/* *** looping *** */

// For each value `val` in cons pair `cons` using iterator `iter`,
// do something
#define SCM_FOREACH(val, cons, iter)                                         \
    for ((iter) = PTR_VAL(cons); !IS_NIL(iter); (iter) = AS_CONS(iter)->cdr) \
        if (((val) = AS_CONS(iter)->car), true)

#endif  // _value_h
