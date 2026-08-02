#include "mem.h"
#include "error.h"

#include <stdlib.h>

uint8_t *
alloc(size_t bytes)
{
        uint8_t *p;

        if (!(p = (uint8_t *)malloc(bytes)))
                fatal("could not alloc %zu bytes", bytes);

        return p;
}
