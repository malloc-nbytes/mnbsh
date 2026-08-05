#ifndef SCOPE_H_INCLUDED
#define SCOPE_H_INCLUDED

#include "variable.h"

MAP_DEFINE(const char *, variable *, varmap);
ARRAY_DEFINE(varmap, varmap_ar);

typedef struct {
        varmap_ar scopes;
        size_t    current;
} scope;

scope     scope_create(void);
void      scope_push(scope *s);
void      scope_insert(scope *s, const char *id, rtv *value);
int       scope_contains(scope *s, const char *id);

// Call scope_contains() before calling this function.
variable *scope_get(scope *, const char *id);
void scope_print(const scope *);

#endif // SCOPE_H_INCLUDED
