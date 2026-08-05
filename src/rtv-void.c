#include "rtv-void.h"

rtv_void *g_rtv_void = NULL;

rtv_void *
rtv_void_alloc(void)
{
        return g_rtv_void;
}


