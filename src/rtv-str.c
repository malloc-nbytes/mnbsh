#include "rtv-str.h"
#include "mem.h"

#include <assert.h>

rtv_str *
rtv_str_alloc(sv view)
{
        rtv_str *v;

        v          = (rtv_str *)alloc(sizeof(*v));
        v->s       = str_from(sv_view(view));
        v->base.ty = (type *)g_type_str;

        return v;
}

void
rtv_str_prepend(rtv_str    *v,
                const char *s)
{
        assert(s);
        for (size_t i = 0; s[i]; ++i)
                str_insert(&v->s, i, s[i]);
}

void
rtv_str_append(rtv_str    *v,
               const char *s)
{
        assert(s);
        str_concat(&v->s, s);
}
