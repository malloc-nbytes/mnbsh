#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

#include "interpreter.h"
#include "rtv.h"
#include "rtv-void.h"
#include "rtv-str.h"
#include "rtv-int.h"
#include "rtv-list.h"
#include "scope.h"
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
        IN_VARDECL  = 1 << 1,
};

typedef struct {
        uint32_t bits;
        cstr_ar  cmds;
} int_context;

scope g_scope;

static void append_value(int_context *ctx, rtv *v);

static void *
interpret_expr_identifier(visitor         *v,
                          expr_identifier *e)
{
        (void)v;

        const char *id;
        variable   *var;

        id = sv_view(e->id->lx);

        if (!(var = scope_get(&g_scope, id)))
                fatal("variable `%s' is not defined", id);

        return var->value;
}

static void *
interpret_expr_str(visitor *v, expr_str *e)
{
        (void)v;
        return rtv_str_alloc(e->s->lx);
}

static void *
interpret_expr_int(visitor *v, expr_int *e)
{
        (void)v;
        return rtv_int_alloc(e->i->lx);
}

static void *
interpret_expr_binary(visitor *v, expr_binary *e)
{
        int_context *ctx = (int_context *)v->context;
        rtv   *lhs = e->lhs->accept(e->lhs, v);
        token *op  = e->op;
        rtv   *rhs = e->rhs->accept(e->rhs, v);

        if (BITSET(*ctx, IN_VARDECL)) {
                assert(lhs->ty->kind == TYPE_KIND_STR);
                assert(rhs->ty->kind == TYPE_KIND_STR);

                rtv_str_prepend((rtv_str *)rhs, sv_view(op->lx));

                type_list *ty  = type_list_alloc((type *)type_str_alloc());
                rtvp_ar values = array_empty();

                array_append(values, lhs);
                array_append(values, rhs);

                return (void *)rtv_list_alloc(values, ty);
        } else if (BITUNSET(*ctx, IN_FUNCTION)) {
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
interpret_stmt_vardecl(visitor *v, stmt_vardecl *s)
{
        int_context *ctx   = (int_context *)v->context;

        SETBIT(*ctx, IN_VARDECL);
        rtv *value = s->e->accept(s->e, v);
        UNSETBIT(*ctx, IN_VARDECL);

        scope_insert(&g_scope, sv_view(s->id->lx), value);

        return NULL;
}

static void *
interpret_stmt_expr(visitor *v, stmt_expr *s)
{
        (void)v;
        return (rtv *)s->e->accept(s->e, v);
}

static void *
interpret_stmt_return(visitor *v, stmt_return *s)
{
        assert(0 && v && s && "interpret_stmt_return: unimplemented");
        return NULL;
}

static void *
interpret_stmt_blk(visitor *v, stmt_blk *s)
{
        assert(0 && v && s && "unimplemented");
        return NULL;
}

static void *
interpret_stmt_func(visitor *v, stmt_func *s)
{
        assert(0 && v && s && "interpret_stmt_func: unimplemented");
        return NULL;
}

static void
execute(int_context *ctx)
{
        pid_t pid;

        #if 1
        for (size_t i = 0; ctx->cmds.data[i]; ++i) {
                if (i != 0) putchar(' ');
                printf("%s", ctx->cmds.data[i]);
        }
        if (ctx->cmds.len > 1)
                putchar('\n');
        #endif

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
append_value(int_context *ctx, rtv *v)
{
        char *res;

        if (!v) {
                array_append(ctx->cmds, NULL);
                return;
        }

        res = NULL;

        switch (v->ty->kind) {
        case TYPE_KIND_STR: {
                res = strdup(((rtv_str *)v)->s.chars);
        } break;
        case TYPE_KIND_LIST: {
                rtv_list *lst = (rtv_list *)v;
                for (size_t i = 0; i < lst->values.len; ++i)
                        append_value(ctx, lst->values.data[i]);
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
        int_context ctx = (int_context) {
                .bits = 0x0000,
                .cmds = array_empty(),
        };

        visitor v = visitor_create((void *)&ctx,
                                   interpret_expr_identifier,
                                   interpret_expr_str,
                                   interpret_expr_int,
                                   interpret_expr_binary,
                                   interpret_stmt_vardecl,
                                   interpret_stmt_expr,
                                   interpret_stmt_return,
                                   interpret_stmt_blk,
                                   interpret_stmt_func);

        for (size_t i = 0; i < p->stmts.len; ++i) {
                stmt *s     = p->stmts.data[i];
                rtv  *value = (rtv *)s->accept(s, &v);
                if (value)
                        append_value(v.context, value);
        }

        append_value((int_context *)v.context, NULL);

        execute(v.context);

        return INTERPRET_RESULT_OK;
}

void
init_interpreter_interface(void)
{
        g_scope = scope_create();
}

#pragma GCC diagnostic pop
