#include "lex.h"
#include "types.h"
#include "interpreter.h"
#include "array.h"
#include "term.h"
#include "str.h"
#include "io.h"
#include "error.h"
#include "glconf.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/wait.h>

#include <readline/readline.h>
#include <readline/history.h>

static void
run(void)
{
        using_history();

        init_lexer_interface();
        init_types_interface();
        init_interpreter_interface();

        while (1) {
                char *src = readline("$ ");

                if (!src)
                        break;

                add_history(src);

                lexer l = lex_file("stdin", src);

                if (!l.err.ok) {
                        fprintf(stderr, "%s error: %s\n",
                                location_ccstr_fmt(l.err.loc),
                                l.err.msg.chars);
                        exit(1);
                }

                parser p = parse(&l);

                interpret(&p);

                parser_destroy(&p);
        }
}

int
main(void)
{
        run();
        return 0;
}
