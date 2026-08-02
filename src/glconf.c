#include <stdint.h>
#include <stddef.h>
#include <termios.h>

struct {
        struct {
                size_t x, y;
                struct termios old;
        } term;
        uint32_t flags;
} glconf = {
        .term = {
                .x = 0,
                .y = 0,
                .old = {0},
        },
        .flags = 0x0000,
};
