#include "lex.h"
#include "kwds.h"
#include "types.h"
#include "error.h"
#include "mem.h"
#include "map.h"

#include <string.h>
#include <ctype.h>
#include <assert.h>

MAP_DEFINE(const char *, token_kind, opmap);
MAP_IMPL  (const char *, token_kind, opmap);

static opmap g_opmap = {0};

static token *
token_alloc(char       *start,
            size_t      len,
            token_kind  k,
            size_t      r,
            size_t      c,
            char       *path)
{
        token *t;

        t       = (token *)alloc(sizeof(*t));
        t->lx   = sv_from(start, (ssize_t)len);
        t->kind = k;
        t->loc  = location_from(sv_from(path, -1), r, c);
        t->next = NULL;

        return t;
}

static void
token_free(token *t)
{
        free(t);
        t = NULL;
}

static void
append(lexer *l, token *t)
{
        array_append(l->tokens, t);
        if (l->tokens.len > 1)
                l->tokens.data[l->tokens.len-2]->next = l->tokens.data[l->tokens.len-1];
}

static size_t
consume_while(char *s, int (*pred)(int))
{
        size_t i;
        for (i = 0; s[i] && pred(s[i]); ++i);
        return i;
}

static int
isident(int c)
{
        return c == '_' || isalnum(c);
}

static int
notsquote(int c)
{
        return c != '\'';
}

static int
notquote(int c)
{
        return c != '"';
}

static int
isop(int c)
{
        return !isspace(c)
                && notsquote(c)
                && notquote(c)
                && !isident(c);
}

const char *
token_kind_ccstr(token_kind k)
{
        switch (k) {
        case TOKEN_KIND_EOF:                return "TOKEN_KIND_EOF";
        case TOKEN_KIND_IDENTIFIER:         return "TOKEN_KIND_IDENTIFIER";
        case TOKEN_KIND_KEYWORD:            return "TOKEN_KIND_KEYWORD";
        case TOKEN_KIND_TYPE:               return "TOKEN_KIND_TYPE";
        case TOKEN_KIND_INTEGER_LITERAL:    return "TOKEN_KIND_INTEGER_LITERAL";
        case TOKEN_KIND_STRING_LITERAL:     return "TOKEN_KIND_STRING_LITERAL";
        case TOKEN_KIND_PLUS:               return "TOKEN_KIND_PLUS";
        case TOKEN_KIND_MINUS:              return "TOKEN_KIND_MINUS";
        case TOKEN_KIND_ASTERISK:           return "TOKEN_KIND_ASTERISK";
        case TOKEN_KIND_FSLASH:             return "TOKEN_KIND_FSLASH";
        case TOKEN_KIND_EQUALS:             return "TOKEN_KIND_EQUALS";
        case TOKEN_KIND_PLUS_EQUALS:        return "TOKEN_KIND_PLUS_EQUALS";
        case TOKEN_KIND_MINUS_EQUALS:       return "TOKEN_KIND_MINUS_EQUALS";
        case TOKEN_KIND_ASTERISK_EQUALS:    return "TOKEN_KIND_ASTERISK_EQUALS";
        case TOKEN_KIND_FSLASH_EQUALS:      return "TOKEN_KIND_FSLASH_EQUALS";
        case TOKEN_KIND_PIPE:               return "TOKEN_KIND_PIPE";
        case TOKEN_KIND_LESSTHAN:           return "TOKEN_KIND_LESSTHAN";
        case TOKEN_KIND_GREATERTHAN:        return "TOKEN_KIND_GREATERTHAN";
        case TOKEN_KIND_LPAREN:             return "TOKEN_KIND_LPAREN";
        case TOKEN_KIND_RPAREN:             return "TOKEN_KIND_RPAREN";
        case TOKEN_KIND_LCURLY:             return "TOKEN_KIND_LCURLY";
        case TOKEN_KIND_RCURLY:             return "TOKEN_KIND_RCURLY";
        case TOKEN_KIND_LSQR:               return "TOKEN_KIND_LSQR";
        case TOKEN_KIND_RSQR:               return "TOKEN_KIND_RSQR";
        case TOKEN_KIND_COMMA:              return "TOKEN_KIND_COMMA";
        case TOKEN_KIND_PERIOD:             return "TOKEN_KIND_PERIOD";
        case TOKEN_KIND_SEMICOLON:          return "TOKEN_KIND_SEMICOLON";
        case TOKEN_KIND_BANG:               return "TOKEN_KIND_BANG";
        case TOKEN_KIND_BANG_EQUALS:        return "TOKEN_KIND_BANG_EQUALS";
        case TOKEN_KIND_DOUBLE_EQUALS:      return "TOKEN_KIND_DOUBLE_EQUALS";
        case TOKEN_KIND_HASH:               return "TOKEN_KIND_HASH";
        case TOKEN_KIND_COLON:              return "TOKEN_KIND_COLON";
        case TOKEN_KIND_DOUBLE_COLON:       return "TOKEN_KIND_DOUBLE_COLON";
        case TOKEN_KIND_QUESTION:           return "TOKEN_KIND_QUESTION";
        case TOKEN_KIND_LESSTHAN_EQUALS:    return "TOKEN_KIND_LESSTHAN_EQUALS";
        case TOKEN_KIND_GREATERTHAN_EQUALS: return "TOKEN_KIND_GREATERTHAN_EQUALS";
        case TOKEN_KIND_DOLLAR:             return "TOKEN_KIND_DOLLAR";
        default:                            return "<unknown>";
        }
}

void
lexer_dump(const lexer *l)
{
        for (size_t i = 0; i < l->tokens.len; ++i) {
                const token *t  = l->tokens.data[i];
                const char  *lx = sv_view(t->lx);
                const char  *k  = token_kind_ccstr(t->kind);

                printf("<lx=%s, k=%s, ", lx, k);

                const char *loc = location_ccstr_fmt(t->loc);

                printf("loc=%s, n=%p>\n", loc, t->next);
        }
}

static unsigned
opmap_hash(const char **s)
{
        return (unsigned)**s;
}

static int
opmap_cmp(const char **s0,
          const char **s1)
{
        return strcmp(*s0, *s1);
}

static token_kind *
determine_op(const char *s,
             size_t     *len)
{
        token_kind *k;
        char buf[32] = {0};

        k = NULL;

        (void)memcpy(buf, s, *len);

        while (*len > 0) {
                if (opmap_contains(&g_opmap, buf))
                        return opmap_get(&g_opmap, buf);
                buf[--(*len)] = 0;
        }

        return k;
}

void
init_lexer_interface(void)
{
        g_opmap = opmap_create(opmap_hash, opmap_cmp);

        opmap_insert(&g_opmap, "+", TOKEN_KIND_PLUS);
        opmap_insert(&g_opmap, "-", TOKEN_KIND_MINUS);
        opmap_insert(&g_opmap, "*", TOKEN_KIND_ASTERISK);
        opmap_insert(&g_opmap, "/", TOKEN_KIND_FSLASH);
        opmap_insert(&g_opmap, "=", TOKEN_KIND_EQUALS);
        opmap_insert(&g_opmap, "+=", TOKEN_KIND_PLUS_EQUALS);
        opmap_insert(&g_opmap, "-=", TOKEN_KIND_MINUS_EQUALS);
        opmap_insert(&g_opmap, "*=", TOKEN_KIND_ASTERISK_EQUALS);
        opmap_insert(&g_opmap, "/=", TOKEN_KIND_FSLASH_EQUALS);
        opmap_insert(&g_opmap, "|", TOKEN_KIND_PIPE);
        opmap_insert(&g_opmap, "<", TOKEN_KIND_LESSTHAN);
        opmap_insert(&g_opmap, ">", TOKEN_KIND_GREATERTHAN);
        opmap_insert(&g_opmap, "(", TOKEN_KIND_LPAREN);
        opmap_insert(&g_opmap, ")", TOKEN_KIND_RPAREN);
        opmap_insert(&g_opmap, "{", TOKEN_KIND_LCURLY);
        opmap_insert(&g_opmap, "}", TOKEN_KIND_RCURLY);
        opmap_insert(&g_opmap, "[", TOKEN_KIND_LSQR);
        opmap_insert(&g_opmap, "]", TOKEN_KIND_RSQR);
        opmap_insert(&g_opmap, ",", TOKEN_KIND_COMMA);
        opmap_insert(&g_opmap, ".", TOKEN_KIND_PERIOD);
        opmap_insert(&g_opmap, ";", TOKEN_KIND_SEMICOLON);
        opmap_insert(&g_opmap, "!", TOKEN_KIND_BANG);
        opmap_insert(&g_opmap, "!=", TOKEN_KIND_BANG_EQUALS);
        opmap_insert(&g_opmap, "==", TOKEN_KIND_DOUBLE_EQUALS);
        opmap_insert(&g_opmap, "#", TOKEN_KIND_HASH);
        opmap_insert(&g_opmap, ":", TOKEN_KIND_COLON);
        opmap_insert(&g_opmap, "::", TOKEN_KIND_DOUBLE_COLON);
        opmap_insert(&g_opmap, "?", TOKEN_KIND_QUESTION);
        opmap_insert(&g_opmap, "<=", TOKEN_KIND_LESSTHAN_EQUALS);
        opmap_insert(&g_opmap, ">=", TOKEN_KIND_GREATERTHAN_EQUALS);
        opmap_insert(&g_opmap, "$", TOKEN_KIND_DOLLAR);
}

void
lexer_destroy(lexer *l)
{
        for (size_t i = 0; i < l->tokens.len; ++i)
                token_free(l->tokens.data[i]);

        array_destroy(l->tokens);
        free(l->src);
        free(l->path);
}

inline token *
lexer_peek(lexer *l)
{
        return l->tokens.data[l->cursor];
}

inline token *
lexer_peekn(lexer *l, size_t n)
{
        assert(l->cursor+n < l->tokens.len);
        return l->tokens.data[l->cursor+n];
}

token *
lexer_next(lexer *l)
{
        if (!lexer_peek(l))
                fatal("lexer_next: out of tokens");
        return l->tokens.data[l->cursor++];
}

void
lexer_discard(lexer *l)
{
        if (l->cursor < l->tokens.len)
                ++l->cursor;
}

lexer
lex_file(const char *path,
         char       *src)
{
        lexer l;
        size_t r, c, i, n;

        l = (lexer) {
                .tokens = array_empty(),
                .cursor = 0,
                .src    = src,
                .path   = strdup(path),
                .err    = {
                        .ok  = 1,
                        .msg = (str) {0},
                        .loc = (location) {0},
                },
        };

        r = 1;
        c = 1;
        i = 0;
        n = strlen(src);

        while (i < n) {
                char ch = l.src[i];
                if (ch == '\n') {
                        ++r;
                        c = 1;
                        ++i;
                } else if (isspace(ch)) {
                        ++c;
                        ++i;
                } else if (ch == '_' || isalpha(ch)) {
                        size_t len = consume_while(src+i, isident);
                        token *t = token_alloc(src+i, len, TOKEN_KIND_IDENTIFIER, r, c, l.path);
                        if (iskwd(sv_view(t->lx)))
                                t->kind = TOKEN_KIND_KEYWORD;
                        if (istype(sv_view(t->lx)))
                                t->kind = TOKEN_KIND_TYPE;
                        append(&l, t);
                        c += len;
                        i += len;
                } else if (isdigit(ch)) {
                        size_t len = consume_while(src+i, isident);
                        token *t = token_alloc(src+i, len, TOKEN_KIND_INTEGER_LITERAL, r, c, l.path);
                        append(&l, t);
                        c += len;
                        i += len;
                } else if (ch == '\'' || ch == '"') {
                        int single = ch == '\'';
                        size_t len = consume_while(src+i+1, single ? notsquote : notquote);
                        token *t = token_alloc(src+i+1, len, TOKEN_KIND_STRING_LITERAL, r, c, l.path);
                        append(&l, t);
                        c += len+2;
                        i += len+2;
                } else {
                        size_t len;
                        token_kind *k;

                        len = consume_while(src+i, isop);
                        if (!(k = determine_op(src+i, &len))) {
                                l.err.ok  = 0;
                                l.err.msg = str_from("invalid operator");
                                l.err.loc = location_from(sv_from(l.path, -1), r, c);
                                goto done;
                        }

                        token *t = token_alloc(src+i, len, *k, r, c, l.path);
                        append(&l, t);
                        c += len;
                        i += len;
                }
        }

 done:
        append(&l, token_alloc("EOF", 3, TOKEN_KIND_EOF, r, c, l.path));
        return l;
}

