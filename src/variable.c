#include "variable.h"
#include "mem.h"

variable *
variable_alloc(const char *id, rtv *value)
{
        variable *var;

        var        = (variable *)alloc(sizeof(*var));
        var->id    = str_from(id);
        var->value = value;

        return var;
}
