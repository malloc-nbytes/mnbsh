#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#define TYPE_VOID "void"
#define TYPE_INT "int"
#define TYPE_STR "str"

#define TYPES_AS_CPL { \
    TYPE_VOID, \
    TYPE_INT,  \
    TYPE_STR,  \
}

typedef enum {
        TYPE_KIND_VOID = 0,
        TYPE_KIND_INT,
        TYPE_KIND_STR,
        TYPE_KIND_LIST,
} type_kind;

typedef struct {
        type_kind kind;
} type;

typedef struct {
        type base;
} type_void;

typedef struct {
        type base;
} type_int;

typedef struct {
        type base;
} type_str;

typedef struct {
        type base;
        type *inner;
} type_list;

extern type_void *g_type_void;
extern type_int *g_type_int;
extern type_str *g_type_str;

int istype(const char *s);

// *Must* call this function before anything else
void init_types_interface(void);

type_void *type_void_alloc(void);
type_int  *type_int_alloc(void);
type_str  *type_str_alloc(void);
type_list *type_list_alloc(type *inner);

#endif // TYPES_H_INCLUDED
