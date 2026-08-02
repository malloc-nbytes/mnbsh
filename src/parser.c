#include "parser.h"
#include "error.h"

#include <assert.h>
#include <string.h>

#define PEEK(p) lexer_peek((p).l)

static void
seterr(parser   *p,
       str       msg,
       location  loc)
{
        p->err.ok  = 0;
        p->err.msg = msg;
        p->err.loc = loc;
}

static token *
expect(parser *p, token_kind k)
{
        token *t;

        if (!(t = PEEK(*p))) {
                seterr(p, str_from("out of tokens"),
                       array_back(p->l->tokens)->loc);
                return NULL;
        }
        if (t->kind != k) {
                seterr(p, str_from_fmt("expected `%s' but got `%s'",
                                       token_kind_ccstr(k),
                                       token_kind_ccstr(t->kind)),
                       t->loc);
                return NULL;
        }
        return lexer_next(p->l);
}

static expr *
parse_primary_expr(parser *p)
{
        expr *left;

        left = NULL;

        while (1) {
                token *hd = PEEK(*p);

                if (!hd)
                        goto done;

                switch (hd->kind) {
                case TOKEN_KIND_DOLLAR: {
                        lexer_discard(p->l);
                        token *i;
                        if (!(i = expect(p, TOKEN_KIND_IDENTIFIER)))
                                return NULL;
                        left      = (expr *)expr_identifier_alloc(i);
                        left->loc = hd->loc;
                } break;
                case TOKEN_KIND_IDENTIFIER: {
                        if (left)
                                goto done;
                        token *s = lexer_next(p->l);
                        if (p->infun) {
                                left      = (expr *)expr_identifier_alloc(s);
                                left->loc = hd->loc;
                        } else {
                                left      = (expr *)expr_str_alloc(s);
                                left->loc = hd->loc;
                        }
                } break;
                case TOKEN_KIND_INTEGER_LITERAL: {
                        if (left)
                                goto done;
                        token *i  = lexer_next(p->l);
                        if (p->infun)
                                left = (expr *)expr_int_alloc(i);
                        else
                                left = (expr *)expr_str_alloc(i);
                        left->loc = hd->loc;
                } break;
                case TOKEN_KIND_STRING_LITERAL: {
                        if (left)
                                goto done;
                        token *s = lexer_next(p->l);
                        left      = (expr *)expr_str_alloc(s);
                        left->loc = hd->loc;
                } break;
                default:
                        goto done;
                }
        }

 done:
        return left;
}

static expr *
parse_additive_expr(parser *p)
{
        expr *lhs = parse_primary_expr(p);
        token *cur = PEEK(*p);
        while (cur && (cur->kind == TOKEN_KIND_PLUS
                       || cur->kind == TOKEN_KIND_MINUS)) {
                token       *op    = lexer_next(p->l);
                expr        *rhs   = parse_primary_expr(p);
                expr_binary *bin   = expr_binary_alloc(lhs, op, rhs);
                ((expr *)bin)->loc = lhs->loc;
                lhs                = (expr *)bin;
                cur                = PEEK(*p);
        }
        return lhs;
}

static expr *
parse_assignment_expr(parser *p)
{
        expr *lhs = parse_additive_expr(p);

        token *cur = PEEK(*p);
        if (!cur) return lhs;

        switch (cur->kind) {
        case TOKEN_KIND_EQUALS: {
                token *op = lexer_next(p->l);
                expr *rhs = parse_assignment_expr(p);
                return (expr *)expr_binary_alloc(lhs, op, rhs);
        }
        default:
                return lhs;
        }
}

static expr *
parse_expr(parser *p)
{
        return parse_assignment_expr(p);
}

static type *
parse_type(parser *p)
{
        token *ty = expect(p, TOKEN_KIND_TYPE);
        if (!strcmp(sv_view(ty->lx), TYPE_INT))
                return (type *)g_type_int;
        return NULL;
}

static stmt_vardecl *
parse_stmt_vardecl(parser *p)
{
        type *ty;
        token *id;
        expr *e;

        if (!(ty = parse_type(p)))
                return NULL;
        if (!(id = expect(p, TOKEN_KIND_IDENTIFIER)))
                return NULL;
        if (!expect(p, TOKEN_KIND_EQUALS))
                return NULL;
        if (!(e = parse_expr(p)))
                return NULL;
        /* if (!expect(p, TOKEN_KIND_SEMICOLON)) */
        /*         return NULL; */

        return stmt_vardecl_alloc(ty, id, e);
}

static stmt_expr *
parse_stmt_expr(parser *p)
{
        expr *e;

        if (!(e = parse_expr(p)))
                return NULL;

        return stmt_expr_alloc(e);
}

static stmt *
parse_stmt(parser *p)
{
        if (PEEK(*p)->kind == TOKEN_KIND_TYPE)
                return (stmt *)parse_stmt_vardecl(p);
        return (stmt *)parse_stmt_expr(p);
}

parser
parse(lexer *l)
{
        parser p;

        p = (parser) {
                .l      = l,
                .cursor = 0,
                .stmts  = array_empty(),
                .infun  = 0,
                .err    = {
                        .ok  = 1,
                        .msg = (str) {0},
                        .loc = (location) {0},
                },
        };


        while (PEEK(p)->kind != TOKEN_KIND_EOF) {
                stmt *s;

                if (!(s = parse_stmt(&p)))
                        fatal("%s", p.err.msg.chars);

                array_append(p.stmts, s);
        }

        return p;
}

void
parser_destroy(parser *p)
{
        p->cursor = 0;
        p->infun  = 0;

        if (!p->err.ok)
                str_destroy(&p->err.msg);

        p->err.ok = 0;

        for (size_t i = 0; i < p->stmts.len; ++i) {
                // TODO: free all statements
        }

        lexer_destroy(p->l);
        array_destroy(p->stmts);
}
