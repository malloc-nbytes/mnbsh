#include "location.h"

#include <stdio.h>

location
location_from(sv     view,
              size_t r,
              size_t c)
{
        return (location) {
                .path = view,
                .r    = r,
                .c    = c,
        };
}

char *
location_ccstr_fmt(location l)
{
        static char buf[512] = {0};

        sprintf(buf, "%s:%zu:%zu:",
                sv_view(l.path),
                l.r, l.c);

        return buf;
}
