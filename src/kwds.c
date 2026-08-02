#include "kwds.h"

#include <string.h>

int
iskwd(const char *s)
{
        static const char *kwds[] = KWD_CPL;

        for (size_t i = 0; i < sizeof(kwds)/sizeof(*kwds); ++i) {
                if (!strcmp(kwds[i], s))
                        return 1;
        }

        return 0;
}
