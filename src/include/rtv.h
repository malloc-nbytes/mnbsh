#ifndef RTV_H_INCLUDED
#define RTV_H_INCLUDED

#include "types.h"
#include "str.h"
#include "sv.h"
#include "array.h"

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

// *Must* call this function before anything else
void init_rtv_interface(void);

rtv_void *rtv_void_alloc(void);
rtv_str  *rtv_str_alloc(sv view);
rtv_int  *rtv_int_alloc(sv view);
void      rtv_str_prepend(rtv_str *v, const char *s);

#endif // RTV_H_INCLUDED
