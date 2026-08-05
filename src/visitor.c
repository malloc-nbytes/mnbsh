#include "visitor.h"

visitor
visitor_create(void                      *context,
               visit_expr_identifier_sig visit_expr_identifier,
               visit_expr_str_sig        visit_expr_str,
               visit_expr_int_sig        visit_expr_int,
               visit_expr_binary_sig     visit_expr_binary,

               visit_stmt_vardecl_sig visit_stmt_vardecl,
               visit_stmt_expr_sig    visit_stmt_expr,
               visit_stmt_return_sig  visit_stmt_return,
               visit_stmt_blk_sig     visit_stmt_blk,
               visit_stmt_func_sig    visit_stmt_func)
{
        return (visitor) {
                .context               = context,
                .visit_expr_identifier = visit_expr_identifier,
                .visit_expr_str        = visit_expr_str,
                .visit_expr_int        = visit_expr_int,
                .visit_expr_binary     = visit_expr_binary,

                .visit_stmt_vardecl    = visit_stmt_vardecl,
                .visit_stmt_expr       = visit_stmt_expr,
                .visit_stmt_return     = visit_stmt_return,
                .visit_stmt_blk        = visit_stmt_blk,
                .visit_stmt_func       = visit_stmt_func,
        };
}

void *
accept_expr_identifier(expr *e, visitor *v)
{
        if (v->visit_expr_identifier)
                return v->visit_expr_identifier(v, (expr_identifier *)e);
        return NULL;
}

void *
accept_expr_str(expr *e, visitor *v)
{
        if (v->visit_expr_str)
                return v->visit_expr_str(v, (expr_str *)e);
        return NULL;
}

void *
accept_expr_int(expr *e, visitor *v)
{
        if (v->visit_expr_int)
                return v->visit_expr_int(v, (expr_int *)e);
        return NULL;
}

void *
accept_expr_binary(expr *e, visitor *v)
{
        if (v->visit_expr_binary)
                return v->visit_expr_binary(v, (expr_binary *)e);
        return NULL;
}

void *
accept_stmt_vardecl(stmt *s, visitor *v)
{
        if (v->visit_stmt_vardecl)
                return v->visit_stmt_vardecl(v, (stmt_vardecl *)s);
        return NULL;
}

void *
accept_stmt_expr(stmt *s, visitor *v)
{
        if (v->visit_stmt_expr)
                return v->visit_stmt_expr(v, (stmt_expr *)s);
        return NULL;
}

void *
accept_stmt_return(stmt *s, visitor *v)
{
        if (v->visit_stmt_return)
                return v->visit_stmt_return(v, (stmt_return *)s);
        return NULL;
}

void *
accept_stmt_blk(stmt *s, visitor *v)
{
        if (v->visit_stmt_blk)
                return v->visit_stmt_blk(v, (stmt_blk *)s);
        return NULL;
}

void *
accept_stmt_func(stmt *s, visitor *v)
{
        if (v->visit_stmt_func)
                return v->visit_stmt_func(v, (stmt_func *)s);
        return NULL;
}
