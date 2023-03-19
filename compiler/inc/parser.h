#ifndef __PARSER_H__
#define __PARSER_H__

#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>

extern void* parser_error;

typedef struct parser parser_t;
typedef struct parser_parse_result parser_parse_result_t;
typedef struct parser_cnext_result parser_cnext_result_t;

struct parser {
    token_t* token;
    ast_context_t* context;
};

struct parser_parse_result {
    ast_context_t* context;
    token_t* error;
};

struct parser_cnext_result {
    token_t* token;
    parser_parse_result_t error;
};

bool parser_skip_space(parser_t* parser);
bool parser_skip_nl(parser_t* parser);
bool parser_skip(parser_t* parser);

void parser_prev(parser_t* parser);
token_t* parser_next(parser_t* parser);
token_t* parser_cnext(parser_t* parser, jmp_buf catch, unsigned int argc, ...);

ast_function_t* parser_parse_function(parser_t* parser, jmp_buf catch);
ast_variable_t* parser_parse_variable(parser_t* parser, jmp_buf catch, bool global);
ast_body_t* parser_parse_body(parser_t* parser, jmp_buf catch, token_type_t open, token_type_t close);
ast_expr_t* parser_parse_expr(parser_t* parser, jmp_buf catch, ast_expr_t* left, token_type_t close);
ast_type_t* parser_parse_type(parser_t* parser, jmp_buf catch);

ast_variable_t* parser_parse_var_or_arg_define(parser_t* parser, jmp_buf catch);

parser_parse_result_t parser_parse(token_t* token);

#endif /* __PARSER_H__ */