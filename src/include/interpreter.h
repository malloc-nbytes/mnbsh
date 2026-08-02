#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED

#include "parser.h"
#include "str.h"
#include "types.h"
#include "array.h"

typedef enum {
        INTERPRET_RESULT_OK = 0,
        INTERPRET_RESULT_RUNTIME_ERROR,
} interpret_result;

typedef struct { type *ty; } rtv;

ARRAY_DEFINE(rtv *, rtvp_ar);

typedef struct {
        rtv base;
} rtv_void;

typedef struct {
        rtv base;
        str s;
} rtv_str;

typedef struct {
        rtv base;
        int i;
} rtv_int;

rtv_void *rtv_void_alloc(void);
rtv_str  *rtv_str_alloc(sv view);
rtv_int  *rtv_int_alloc(sv view);

// *Must* call this function before anything else
void             init_interpreter_interface(void);
interpret_result interpret(parser *p);

#endif // INTERPRETER_H_INCLUDED
