#ifndef GLCONF_H_INCLUDED
#define GLCONF_H_INCLUDED

#include <stdint.h>
#include <termios.h>

extern struct {
        struct {
                size_t x, y;
                struct termios old;
        } term;
        uint32_t flags;
} glconf;

#endif // GLCONF_H_INCLUDED
