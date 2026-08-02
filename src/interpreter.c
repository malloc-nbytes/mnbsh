#include "interpreter.h"
#include "visitor.h"

#include <assert.h>

typedef struct {
        int infun;
} interp_context;

static void *
interpret_expr_identifier(visitor         *v,
                          expr_identifier *e)
{
        assert(0 && v && e && "unimplemented");
        return NULL;
}

static void *
interpret_expr_str(visitor  *v,
                   expr_str *e)
{
        assert(0 && v && e && "unimplemented");
        return NULL;
}

static void *
interpret_expr_int(visitor  *v,
                   expr_int *e)
{
        assert(0 && v && e && "unimplemented");
        return NULL;
}

static void *
interpret_expr_binary(visitor     *v,
                      expr_binary *e)
{
        assert(0 && v && e && "unimplemented");
        return NULL;
}

static void *
interpret_stmt_vardecl(visitor      *v,
                       stmt_vardecl *s)
{
        assert(0 && v && s && "unimplemented");
        return NULL;
}

static void *
interpret_stmt_expr(visitor   *v,
                    stmt_expr *s)
{
        assert(0 && v && s && "unimplemented");
        return NULL;
}

void
interpret(parser *p)
{
        interp_context ctx = (interp_context) {
                .infun = 0,
        };

        visitor v = visitor_create(&ctx,
                                   interpret_expr_identifier,
                                   interpret_expr_str,
                                   interpret_expr_int,
                                   interpret_expr_binary,
                                   interpret_stmt_vardecl,
                                   interpret_stmt_expr);


        for (size_t i = 0; i < p->stmts.len; ++i) {
                assert(0);
                p->stmts.data[i]->accept(p->stmts.data[i], &v);
        }
}
