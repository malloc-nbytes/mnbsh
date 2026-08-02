#include "interpreter.h"
#include "visitor.h"
#include "mem.h"
#include "error.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BITSET(ctx, f)   (((ctx).bits & (f)) != 0)
#define SETBIT(ctx, f)   ((ctx).bits |= (f))
#define UNSETBIT(ctx, f) ((ctx).bits &= ~(f))

rtv_void *g_rtv_void = NULL;

enum {
        IN_FUNCTION = 1 << 0,
};

typedef struct {
        uint32_t bits;
        cstr_ar  cmds;
} interpreter_context;

rtv_void *
rtv_void_alloc(void)
{
        return g_rtv_void;
}

rtv_str *
rtv_str_alloc(sv view)
{
        rtv_str *v;

        v          = (rtv_str *)alloc(sizeof(*v));
        v->s       = str_from(sv_view(view));
        v->base.ty = (type *)g_type_str;

        return v;
}

rtv_int *
rtv_int_alloc(sv view)
{
        rtv_int *v;

        v          = (rtv_int *)alloc(sizeof(*v));
        v->i       = atoi(sv_view(view));
        v->base.ty = (type *)g_type_int;

        return v;
}

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
        (void)v;
        (void)e->lhs->accept(e->lhs, v);
        (void)e->rhs->accept(e->rhs, v);
        return NULL;
}

static void *
visit_stmt_vardecl(visitor *v, stmt_vardecl *s)
{
        (void)v;
        (void)s;
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

        array_append(((interpreter_context *)v.context)->cmds, NULL);

        execute(v.context);

        return INTERPRET_RESULT_OK;
}

void
init_interpreter_interface(void)
{
        g_rtv_void = (rtv_void *)alloc(sizeof(*g_rtv_void));
        g_rtv_void->base.ty = (type *)g_type_void;
}
