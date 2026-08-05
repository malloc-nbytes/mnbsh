#ifndef RTV_INT_H_INCLUDED
#define RTV_INT_H_INCLUDED

#include "rtv.h"

typedef struct {
        rtv base;
        int i;
} rtv_int;

rtv_int  *rtv_int_alloc(sv view);

#endif // RTV_INT_H_INCLUDED
