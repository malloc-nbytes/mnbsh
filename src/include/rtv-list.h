#ifndef RTV_LIST_H_INCLUDED
#define RTV_LIST_H_INCLUDED

#include "rtv.h"

typedef struct {
        rtv     base;
        rtvp_ar values;
} rtv_list;

rtv_list *rtv_list_alloc(rtvp_ar values, type_list *ty);

#endif //RTV_LIST_H_INCLUDED
