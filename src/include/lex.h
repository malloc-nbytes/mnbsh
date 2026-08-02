#ifndef LEX_H_INCLUDED
#define LEX_H_INCLUDED

#include "location.h"
#include "sv.h"
#include "str.h"

typedef enum {
        TOKEN_KIND_EOF = 0,
        TOKEN_KIND_IDENTIFIER,
        TOKEN_KIND_KEYWORD,
        TOKEN_KIND_TYPE,
        TOKEN_KIND_INTEGER_LITERAL,
        TOKEN_KIND_STRING_LITERAL,

        TOKEN_KIND_PLUS,
        TOKEN_KIND_MINUS,
        TOKEN_KIND_ASTERISK,
        TOKEN_KIND_FSLASH,
        TOKEN_KIND_EQUALS,

        TOKEN_KIND_PLUS_EQUALS,
        TOKEN_KIND_MINUS_EQUALS,
        TOKEN_KIND_ASTERISK_EQUALS,
        TOKEN_KIND_FSLASH_EQUALS,

        TOKEN_KIND_PIPE,
        TOKEN_KIND_LESSTHAN,
        TOKEN_KIND_GREATERTHAN,

        TOKEN_KIND_LPAREN,
        TOKEN_KIND_RPAREN,
        TOKEN_KIND_LCURLY,
        TOKEN_KIND_RCURLY,
        TOKEN_KIND_LSQR,
        TOKEN_KIND_RSQR,
        TOKEN_KIND_COMMA,
        TOKEN_KIND_PERIOD,
        TOKEN_KIND_SEMICOLON,
        TOKEN_KIND_BANG,
        TOKEN_KIND_BANG_EQUALS,
        TOKEN_KIND_DOUBLE_EQUALS,

        TOKEN_KIND_HASH,
        TOKEN_KIND_COLON,
        TOKEN_KIND_DOUBLE_COLON,
        TOKEN_KIND_QUESTION,

        TOKEN_KIND_LESSTHAN_EQUALS,
        TOKEN_KIND_GREATERTHAN_EQUALS,

        TOKEN_KIND_DOLLAR,
} token_kind;

typedef struct token {
        sv            lx;
        token_kind    kind;
        location      loc;
        struct token *next;
} token;

ARRAY_DEFINE(token *, tokenp_ar);

typedef struct {
        tokenp_ar  tokens;
        size_t     cursor;
        char      *src;
        char      *path;
        struct {
                int      ok;
                str      msg;
                location loc;
        } err;
} lexer;

// *Must* call this function before anything else
void init_lexer_interface(void);

// Takes ownership over `src'.
lexer lex_file(const char *path, char *src);
void  lexer_dump(const lexer *l);
void  lexer_destroy(lexer *l);
token *lexer_peek(lexer *l);
token *lexer_peekn(lexer *l, size_t n);
token *lexer_next(lexer *l);
const char *token_kind_ccstr(token_kind k);
void lexer_discard(lexer *l);

#endif // LEX_H_INCLUDED
