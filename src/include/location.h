#ifndef LOCATION_H_INCLUDED
#define LOCATION_H_INCLUDED

#include "sv.h"

typedef struct {
        sv path;
        size_t r;
        size_t c;
} location;

location  location_from(sv view, size_t r, size_t c);
char     *location_ccstr_fmt(location l);

#endif // LOCATION_H_INCLUDED
