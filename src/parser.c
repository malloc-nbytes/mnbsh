#include "parser.h"
#include "kwds.h"
#include "error.h"

#include <assert.h>
#include <string.h>

#define PEEK(p) lexer_peek((p).l)

static stmt *parse_stmt(parser *p);

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

static token *
expectkw(parser *p, const char *kwd)
{
        token *t;

        if (!(t = expect(p, TOKEN_KIND_KEYWORD))) {
                seterr(p, str_from_fmt("expected `%s' but got `%s'", kwd, token_kind_ccstr(t->kind)),
                       array_back(p->l->tokens)->loc);
        }

        if (strcmp(sv_view(t->lx), kwd)) {
                seterr(p, str_from_fmt("expected keyword `%s' but got `%s'",
                                       kwd, token_kind_ccstr(t->kind)),
                       t->loc);
                return NULL;
        }

        return t;
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
        token *ty;

        if (!(ty = expect(p, TOKEN_KIND_TYPE)))
                return NULL;
        if (!strcmp(sv_view(ty->lx), TYPE_INT))
                return (type *)g_type_int;
        if (!strcmp(sv_view(ty->lx), TYPE_STR))
                return (type *)g_type_str;
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

static stmt_return *
parse_stmt_return(parser *p)
{
        const token *t;
        expr *e;

        if (!(t = expectkw(p, KWD_RETURN)))
                return NULL;

        e = parse_expr(p);

        return stmt_return_alloc(e, t->loc);
}

static stmt_blk *
parse_stmt_blk(parser *p)
{
        const token *start = expect(p, TOKEN_KIND_LCURLY);
        stmtp_ar ar        = array_empty();

        while (PEEK(*p)->kind != TOKEN_KIND_RCURLY) {
                stmt *s;

                if (!(s = parse_stmt(p)))
                        return NULL;

                array_append(ar, s);
        }

        if (!expect(p, TOKEN_KIND_RCURLY))
                return NULL;

        return stmt_blk_alloc(ar, start->loc);
}

static int
parse_function_params(parser *p, param_ar *ar)
{
        array_zero(*ar);

        while (PEEK(*p)->kind == TOKEN_KIND_TYPE) {
                type *ty;
                token *id;

                if (!(ty = parse_type(p)))
                        return 0;

                if (!(id = expect(p, TOKEN_KIND_IDENTIFIER)))
                        return 0;

                array_append(*ar, ((param) { .ty = ty, .id = id }));
        }

        return 1;
}

static stmt_func *
parse_stmt_func(parser *p)
{
        type *rty;
        token *id;
        param_ar params;
        stmt_blk *body;

        if (!expectkw(p, KWD_FUNC))
                return NULL;

        if (!(rty = parse_type(p)))
                return NULL;

        if (!(id = expect(p, TOKEN_KIND_IDENTIFIER)))
                return NULL;

        if (!expect(p, TOKEN_KIND_LPAREN))
                return NULL;

        if (!parse_function_params(p, &params))
                return NULL;

        if (!expect(p, TOKEN_KIND_RPAREN))
                return NULL;

        if (!(body = parse_stmt_blk(p)))
                return NULL;

        return stmt_func_alloc(rty, id, params, body);
}

static stmt *
parse_stmt_keyword(parser *p)
{
        const token *hd  = PEEK(*p);
        const char  *kwd = sv_view(hd->lx);

        if (!strcmp(kwd, KWD_RETURN))
                return (stmt *)parse_stmt_return(p);
        if (!strcmp(kwd, KWD_FUNC))
                return (stmt *)parse_stmt_func(p);

        seterr(p, str_from_fmt("illegal keyword `%s'", kwd), hd->loc);

        return NULL;
}

static stmt *
parse_stmt(parser *p)
{
        if (PEEK(*p)->kind == TOKEN_KIND_KEYWORD)
                return parse_stmt_keyword(p);
        if (PEEK(*p)->kind == TOKEN_KIND_TYPE)
                return (stmt *)parse_stmt_vardecl(p);
        if (PEEK(*p)->kind == TOKEN_KIND_LCURLY)
                return (stmt *)parse_stmt_blk(p);
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
