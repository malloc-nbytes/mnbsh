#include "types.h"
#include "mem.h"

#include <string.h>

type_void *g_type_void = NULL;
type_int  *g_type_int  = NULL;
type_str  *g_type_str  = NULL;

int
istype(const char *s)
{
        static const char *types[] = TYPES_AS_CPL;

        for (size_t i = 0; i < sizeof(types)/sizeof(*types); ++i) {
                if (!strcmp(types[i], s))
                        return 1;
        }

        return 0;
}

static type_void *
type_void_alloc(void)
{
        return g_type_void;
}

static type_int *
type_int_alloc(void)
{
        return g_type_int;
}

static type_str *
type_str_alloc(void)
{
        return g_type_str;
}

static type_list *
type_list_alloc(type *inner)
{
        type_list *ty;

        ty            = (type_list *)alloc(sizeof(*ty));
        ty->base.kind = TYPE_KIND_LIST;
        ty->inner     = inner;

        return ty;
}

void
init_types_interface(void)
{
        (void)type_list_alloc;
        (void)type_str_alloc;
        (void)type_int_alloc;
        (void)type_void_alloc;

        g_type_void            = (type_void *)alloc(sizeof(*g_type_void));
        g_type_void->base.kind = TYPE_KIND_VOID;
        g_type_int             = (type_int *)alloc(sizeof(*g_type_int));
        g_type_int->base.kind  = TYPE_KIND_INT;
        g_type_str             = (type_str *)alloc(sizeof(*g_type_str));
        g_type_str->base.kind  = TYPE_KIND_STR;
}
