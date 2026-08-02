#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include "lex.h"
#include "grammar.h"
#include "str.h"
#include "location.h"

#include <stddef.h>

typedef struct {
        lexer *l;
        size_t cursor;
        stmtp_ar stmts;
        int infun;
        struct {
                int ok;
                str msg;
                location loc;
        } err;
} parser;

parser parse(lexer *l);
void parser_destroy(parser *p);

#endif // PARSER_H_INCLUDED
