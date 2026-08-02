#ifndef KWDS_H_INCLUDED
#define KWDS_H_INCLUDED

#define KWD_IF    "if"
#define KWD_ELSE  "else"
#define KWD_WHILE "while"
#define KWD_FOR   "for"

#define KWD_CPL {  \
        KWD_IF,    \
        KWD_ELSE,  \
        KWD_WHILE, \
        KWD_FOR,   \
}

int iskwd(const char *s);

#endif // KWDS_H_INCLUDED
