#include "rtv-list.h"
#include "mem.h"

rtv_list *
rtv_list_alloc(rtvp_ar values, type_list *ty)
{
        rtv_list *v;

        v          = (rtv_list *)alloc(sizeof(*v));
        v->values  = values;
        v->base.ty = (type *)ty;

        return v;
}


