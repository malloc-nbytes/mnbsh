#include "rtv.h"
#include "mem.h"

#include <assert.h>

rtv_void *g_rtv_void = NULL;

void
init_rtv_interface(void)
{
        g_rtv_void          = (rtv_void *)alloc(sizeof(*g_rtv_void));
        g_rtv_void->base.ty = (type *)g_type_void;
}

rtv_void *
rtv_void_alloc(void)
{
        return g_rtv_void;
}

rtv_str *
rtv_str_alloc(sv view)
{
        rtv_str *v;

        v          = (rtv_str *)alloc(sizeof(*v));
        v->s       = str_from(sv_view(view));
        v->base.ty = (type *)g_type_str;

        return v;
}

rtv_int *
rtv_int_alloc(sv view)
{
        rtv_int *v;

        v          = (rtv_int *)alloc(sizeof(*v));
        v->i       = atoi(sv_view(view));
        v->base.ty = (type *)g_type_int;

        return v;
}

void
rtv_str_prepend(rtv_str *v, const char *s)
{
        assert(s);
        for (size_t i = 0; s[i]; ++i)
                str_insert(&v->s, i, s[i]);
}
