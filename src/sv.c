#include "sv.h"

#include <string.h>
#include <assert.h>

sv
sv_from(const char *s,
        ssize_t     len)
{
        return (sv) {
                .s = s,
                .len = len == -1 ? strlen(s) : (size_t)len,
        };
}

const char *
sv_ccstr(sv view)
{
        assert(view.len < 512-1);

        static char buf[512] = {0};

        (void)memcpy(buf, view.s, view.len);
        buf[view.len] = 0;

        return buf;
}
