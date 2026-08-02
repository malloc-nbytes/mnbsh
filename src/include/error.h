#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#include <stdio.h>

#define fatal(msg, ...) \
        do { \
                fprintf(stderr, "fatal: " msg "\n", ##__VA_ARGS__); \
                exit(1); \
        } while (0)

#endif // ERROR_H_INCLUDED
