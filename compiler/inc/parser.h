#ifndef __PARSER_H__
#define __PARSER_H__

#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>

typedef struct parser parser_t;
typedef struct parser_parse_result parser_parse_result_t;
typedef struct parser_cnext_result parser_cnext_result_t;

struct parser {
    // Вход
    token_t* token;
    // Выход
    ast_context_t* context;
    //
    jmp_buf catch;
    token_t* error;
};

struct parser_parse_result {
    ast_context_t* context;
    token_t* error;
};

struct parser_cnext_result {
    token_t* token;
    parser_parse_result_t error;
};

__attribute__((__noreturn__))
void parser_throw(token_t* token);

bool parser_skip_space();
bool parser_skip_nl();
bool parser_skip();

void parser_prev();
token_t* parser_next();
token_t* parser_cnext(unsigned int argc, ...);

ast_function_t* parser_parse_function();
ast_variable_t* parser_parse_variable(bool global);
ast_body_t* parser_parse_body(token_type_t open, token_type_t close);
ast_expr_t* parser_parse_expr(ast_expr_t* left);
ast_type_t* parser_parse_type();

ast_variable_t* parser_parse_var_or_arg_define();

parser_parse_result_t parser_parse(token_t* token);

#endif /* __PARSER_H__ */