#ifndef RTV_STR_H_INCLUDED
#define RTV_STR_H_INCLUDED

#include "rtv.h"

typedef struct {
        rtv base;
        str s;
} rtv_str;

rtv_str *rtv_str_alloc(sv view);
void     rtv_str_prepend(rtv_str *v, const char *s);
void     rtv_str_append(rtv_str *v, const char *s);


#endif // RTV_STR_H_INCLUDED
