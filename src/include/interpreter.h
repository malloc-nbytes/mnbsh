#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED

#include "parser.h"
#include "str.h"
#include "array.h"

typedef enum {
        INTERPRET_RESULT_OK = 0,
        INTERPRET_RESULT_RUNTIME_ERROR,
} interpret_result;

interpret_result interpret(parser *p);
void             init_interpreter_interface(void);

#endif // INTERPRETER_H_INCLUDED
