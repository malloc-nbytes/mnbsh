#include "interpreter.h"
#include "rtv.h"
#include "visitor.h"
#include "mem.h"
#include "error.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BITSET(ctx, f)   (((ctx).bits & (f)) != 0)
#define BITUNSET(ctx, f) (((ctx).bits & (f)) == 0)
#define SETBIT(ctx, f)   ((ctx).bits |= (f))
#define UNSETBIT(ctx, f) ((ctx).bits &= ~(f))

enum {
        IN_FUNCTION = 1 << 0,
};

typedef struct {
        uint32_t bits;
        cstr_ar  cmds;
} interpreter_context;

static void append_value(interpreter_context *ctx, rtv *v);

static void *
visit_expr_identifier(visitor         *v,
                      expr_identifier *e)
{
        assert(0 && v && e);
        return NULL;
}

static void *
visit_expr_str(visitor *v, expr_str *e)
{
        (void)v;
        //printf("STR: %s\n", sv_view(e->s->lx));
        return rtv_str_alloc(e->s->lx);
}

static void *
visit_expr_int(visitor *v, expr_int *e)
{
        (void)v;
        return rtv_int_alloc(e->i->lx);
}

static void *
visit_expr_binary(visitor *v, expr_binary *e)
{
        interpreter_context *ctx = (interpreter_context *)v->context;
        rtv   *lhs = e->lhs->accept(e->lhs, v);
        token *op  = e->op;
        rtv   *rhs = e->rhs->accept(e->rhs, v);

        if (BITUNSET(*ctx, IN_FUNCTION)) {
                assert(rhs->ty->kind == TYPE_KIND_STR);
                append_value(ctx, lhs);
                rtv_str_prepend((rtv_str *)rhs, sv_view(op->lx));
                append_value(ctx, rhs);
        } else {
                assert(0 && "unimplemented");
        }


        return NULL;
}

static void *
visit_stmt_vardecl(visitor *v, stmt_vardecl *s)
{
        assert(0 && v && s && "unimplemented");
        return NULL;
}

static void *
visit_stmt_expr(visitor *v, stmt_expr *s)
{
        (void)v;
        return (rtv *)s->e->accept(s->e, v);
}

static void
execute(interpreter_context *ctx)
{
        pid_t pid;

        pid = fork();

        if (pid == 0) {
                // child
                execvp(ctx->cmds.data[0], ctx->cmds.data);
                perror("execvp");
                _exit(255);
        } else if (pid > 0) {
                // parent
                waitpid(pid, NULL, 0);
        }
        else
                perror("fork");
}

static void
append_value(interpreter_context *ctx, rtv *v)
{
        char *res;

        if (!v) {
                array_append(ctx->cmds, NULL);
                return;
        }

        switch (v->ty->kind) {
        case TYPE_KIND_STR: {
                res = strdup(((rtv_str *)v)->s.chars);
        } break;
        default:
                fatal("unhandled type '%d'", v->ty->kind);
                break;
        }

        array_append(ctx->cmds, res);
}

interpret_result
interpret(parser *p)
{
        interpreter_context ctx = (interpreter_context) {
                .bits = 0x0000,
                .cmds = array_empty(),
        };

        visitor v = visitor_create((void *)&ctx,
                                   visit_expr_identifier,
                                   visit_expr_str,
                                   visit_expr_int,
                                   visit_expr_binary,
                                   visit_stmt_vardecl,
                                   visit_stmt_expr);

        for (size_t i = 0; i < p->stmts.len; ++i) {
                stmt *s = p->stmts.data[i];
                rtv *value = (rtv *)s->accept(s, &v);
                if (value)
                        append_value(v.context, value);
        }

        append_value((interpreter_context *)v.context, NULL);

        execute(v.context);

        return INTERPRET_RESULT_OK;
}
