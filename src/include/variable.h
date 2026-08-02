#ifndef VARIABLE_H_INCLUDED
#define VARIABLE_H_INCLUDED

#include "str.h"
#include "rtv.h"
#include "array.h"
#include "map.h"

typedef struct {
        str id;
        rtv *value;
} variable;

variable *variable_alloc(const char *id, rtv *value);

#endif // VARIABLE_H_INCLUDED
