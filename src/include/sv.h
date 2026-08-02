#ifndef SV_H_INCLUDED
#define SV_H_INCLUDED

#include <stddef.h>
#include <sys/types.h>

typedef struct {
        const char *s;
        size_t      len;
} sv;

sv          sv_from(const char *s, ssize_t len);
const char *sv_ccstr(sv view);

#endif // SV_H_INCLUDED
