#ifndef RTV_VOID_H_INCLUDED
#define RTV_VOID_H_INCLUDED

#include "rtv.h"

typedef struct {
        rtv base;
} rtv_void;

extern rtv_void *g_rtv_void;

rtv_void *rtv_void_alloc(void);

#endif // RTV_VOID_H_INCLUDED
