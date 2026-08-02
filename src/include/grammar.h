#ifndef GRAMMAR_H_INCLUDED
#define GRAMMAR_H_INCLUDED

#include "lex.h"
#include "array.h"
#include "types.h"
#include "location.h"

typedef struct visitor visitor;

typedef enum {
        EXPR_KIND_IDENTIFIER = 0,
        EXPR_KIND_STR,
        EXPR_KIND_INT,
        EXPR_KIND_BINARY,
} expr_kind;

typedef enum {
        STMT_KIND_VARDECL = 0,
        STMT_KIND_EXPR,
} stmt_kind;

typedef struct expr {
        expr_kind kind;
        location loc;
        void *(*accept)(struct expr *, visitor *);
} expr;

typedef struct stmt {
        stmt_kind kind;
        location loc;
        void *(*accept)(struct stmt *, visitor *);
} stmt;

ARRAY_DEFINE(stmt *, stmtp_ar);

typedef struct {
        expr base;
        token *id;
} expr_identifier;

typedef struct {
        expr base;
        token *s;
} expr_str;

typedef struct {
        expr base;
        token *i;
} expr_int;

typedef struct {
        expr base;
        expr *lhs;
        token *op;
        expr *rhs;
} expr_binary;

typedef struct {
        stmt base;
        type *ty;
        token *id;
        expr *e;
} stmt_vardecl;

typedef struct {
        stmt base;
        expr *e;
} stmt_expr;

expr_identifier *expr_identifier_alloc(token *id);
expr_str        *expr_str_alloc(token *s);
expr_int        *expr_int_alloc(token *i);
expr_binary     *expr_binary_alloc(expr *lhs, token *op, expr *rhs);

stmt_vardecl *stmt_vardecl_alloc(type *ty, token *id, expr *e);
stmt_expr    *stmt_expr_alloc(expr *e);

#endif // GRAMMAR_H_INCLUDED
