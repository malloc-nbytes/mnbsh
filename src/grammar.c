#include "grammar.h"
#include "visitor.h"
#include "mem.h"

expr_identifier *
expr_identifier_alloc(token *id)
{
        expr_identifier *e = (expr_identifier *)alloc(sizeof(*e));

        e->id          = id;
        e->base.kind   = EXPR_KIND_IDENTIFIER;
        e->base.loc    = id->loc;
        e->base.accept = accept_expr_identifier;

        return e;
}

expr_str *
expr_str_alloc(token *s)
{
        expr_str *e = (expr_str *)alloc(sizeof(*e));

        e->s           = s;
        e->base.kind   = EXPR_KIND_STR;
        e->base.loc    = s->loc;
        e->base.accept = accept_expr_str;

        return e;
}

expr_int *
expr_int_alloc(token *i)
{
        expr_int *e = (expr_int *)alloc(sizeof(*e));

        e->i           = i;
        e->base.kind   = EXPR_KIND_INT;
        e->base.loc    = i->loc;
        e->base.accept = accept_expr_int;

        return e;
}

expr_binary *
expr_binary_alloc(expr *lhs, token *op, expr *rhs)
{
        expr_binary *e = (expr_binary *)alloc(sizeof(*e));

        e->lhs         = lhs;
        e->op          = op;
        e->rhs         = rhs;
        e->base.kind   = EXPR_KIND_INT;
        e->base.loc    = lhs->loc;
        e->base.accept = accept_expr_binary;

        return e;
}

stmt_vardecl *
stmt_vardecl_alloc(type *ty, token *id, expr *e)
{
        stmt_vardecl *s = (stmt_vardecl *)alloc(sizeof(*s));
        s->ty           = ty;
        s->id           = id;
        s->e            = e;
        s->base.kind    = STMT_KIND_VARDECL;
        s->base.loc     = id->loc;
        s->base.accept  = accept_stmt_vardecl;
        return s;
}

stmt_expr *
stmt_expr_alloc(expr *e)
{
        stmt_expr *s    = (stmt_expr *)alloc(sizeof(*s));
        s->e            = e;
        s->base.kind    = STMT_KIND_EXPR;
        s->base.loc     = e->loc;
        s->base.accept  = accept_stmt_expr;
        return s;
}

stmt_return *
stmt_return_alloc(expr *e, location loc)
{
        stmt_return *s  = (stmt_return *)alloc(sizeof(*s));
        s->e            = e;
        s->base.kind    = STMT_KIND_RETURN;
        s->base.loc     = loc;
        s->base.accept  = accept_stmt_return;
        return s;
}

stmt_blk *
stmt_blk_alloc(stmtp_ar stmts, location loc)
{
        stmt_blk *s     = (stmt_blk *)alloc(sizeof(*s));
        s->stmts        = stmts;
        s->base.kind    = STMT_KIND_BLK;
        s->base.loc     = loc;
        s->base.accept  = accept_stmt_blk;
        return s;
}

stmt_func *
stmt_func_alloc(type     *rty,
                token    *id,
                param_ar  params,
                stmt_blk *body)
{
        stmt_func *s    = (stmt_func *)alloc(sizeof(*s));
        s->rty          = rty;
        s->id           = id;
        s->params       = params;
        s->body         = body;
        s->base.kind    = STMT_KIND_FUNC;
        s->base.loc     = id->loc;
        s->base.accept  = accept_stmt_func;
        return s;
}
