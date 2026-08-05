#include "scope.h"

#include <string.h>

MAP_IMPL(const char *, variable *, varmap);

static unsigned
varmap_hash(const char **k)
{
        return (unsigned)**k;
}

static int
varmap_cmp(const char **k0,
           const char **k1)
{
        return strcmp(*k0, *k1);
}

scope
scope_create(void)
{
        scope s = (scope) {
                .scopes  = array_empty(),
                .current = 0,
        };

        scope_push(&s);

        s.current = 0;

        return s;
}

void
scope_push(scope *s)
{
        array_append(s->scopes,
                     varmap_create(varmap_hash, varmap_cmp));
        ++s->current;
}

void
scope_insert(scope *s, const char *id, rtv *value)
{
        variable *var = variable_alloc(id, value);

        varmap_insert(&s->scopes.data[s->current], var->id.chars, var);
}

int
scope_contains(scope *s, const char *id)
{
        return scope_get(s, id) != NULL;
}

variable *
scope_get(scope *s, const char *id)
{
        for (int i = (int)s->current; i >= 0; --i) {
                varmap *m = &s->scopes.data[i];
                if (varmap_contains(m, id))
                        return *varmap_get(m, id);
        }
        return NULL;
}

void
scope_print(const scope *s)
{
        for (int i = (int)s->current; i >= 0; --i) {
                varmap_print(&s->scopes.data[i]);
        }
}
