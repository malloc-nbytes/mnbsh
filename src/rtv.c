#include "rtv.h"
#include "rtv-void.h"
#include "mem.h"

#include <assert.h>

void
init_rtv_interface(void)
{
        g_rtv_void          = (rtv_void *)alloc(sizeof(*g_rtv_void));
        g_rtv_void->base.ty = (type *)g_type_void;
}

