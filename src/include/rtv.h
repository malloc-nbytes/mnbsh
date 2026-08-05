#ifndef RTV_H_INCLUDED
#define RTV_H_INCLUDED

#include "types.h"
#include "str.h"
#include "sv.h"
#include "array.h"

typedef struct { type *ty; } rtv;

ARRAY_DEFINE(rtv *, rtvp_ar);

// *Must* call this function before anything else
void init_rtv_interface(void);

#endif // RTV_H_INCLUDED
