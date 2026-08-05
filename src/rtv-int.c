#include "rtv-int.h"
#include "mem.h"

rtv_int *
rtv_int_alloc(sv view)
{
        rtv_int *v;

        v          = (rtv_int *)alloc(sizeof(*v));
        v->i       = atoi(sv_view(view));
        v->base.ty = (type *)g_type_int;

        return v;
}


