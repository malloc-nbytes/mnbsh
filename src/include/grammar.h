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
        STMT_KIND_RETURN,
        STMT_KIND_BLK,
        STMT_KIND_FUNC,
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

typedef struct {
        stmt base;
        expr *e; // can be null for no value
} stmt_return;

typedef struct {
        stmt     base;
        stmtp_ar stmts;
} stmt_blk;

typedef struct {
        type *ty;
        token *id;
} param;
ARRAY_DEFINE(param, param_ar);

typedef struct {
        stmt      base;
        type     *rty;
        token    *id;
        param_ar  params;
        stmt_blk *body;
} stmt_func;

expr_identifier *expr_identifier_alloc(token *id);
expr_str        *expr_str_alloc(token *s);
expr_int        *expr_int_alloc(token *i);
expr_binary     *expr_binary_alloc(expr *lhs, token *op, expr *rhs);

stmt_vardecl *stmt_vardecl_alloc(type *ty, token *id, expr *e);
stmt_expr    *stmt_expr_alloc(expr *e);
stmt_return  *stmt_return_alloc(expr *e, location loc);
stmt_blk     *stmt_blk_alloc(stmtp_ar stmts, location loc);
stmt_func    *stmt_func_alloc(type *rty, token *id, param_ar params, stmt_blk *body);

#endif // GRAMMAR_H_INCLUDED
