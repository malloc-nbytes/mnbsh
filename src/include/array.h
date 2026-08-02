#ifndef ARRAY_H_INCLUDED
#define ARRAY_H_INCLUDED

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#define ARRAY_DEFINE(ty, name)                  \
        typedef struct {                        \
                ty *data;                       \
                size_t len, cap;                \
        } name

#define array_append(ar, d) \
        do { \
                if ((ar).len >= (ar).cap) { \
                        (ar).cap = (ar).cap == 0 ? 2 : (ar).cap*2; \
                        if (!((ar).data = (typeof((ar).data))realloc((ar).data, (ar).cap * sizeof(*(ar).data)))) { \
                                fprintf(stderr, "could not realloc array\n"); \
                                exit(1); \
                        } \
                } \
                (ar).data[(ar).len++] = d; \
        } while (0)

#define array_empty() {0}

#define array_back(ar) (ar).data[(ar).len-1]

#define array_destroy(ar) \
        do { \
                free((ar).data); \
                (ar).len = 0; \
                (ar).cap = 0; \
        } while (0)

ARRAY_DEFINE(char *, cstr_ar);

#endif // ARRAY_H_INCLUDED
