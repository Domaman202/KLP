#ifndef __PARSER_H__
#define __PARSER_H__

#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>

extern token_t* parser_token;

bool parser_skip_space();
bool parser_skip_nl();
bool parser_skip();

void parser_prev();
token_t* parser_next();
#define parser_cnext(argc, ...) parser_cnext__(THROW_INFO, argc, __VA_ARGS__)
token_t* parser_cnext__(char* __fun, char* __file, uint16_t __line, unsigned int argc, ...);

ast_context_t* parser_parse_context(bool inbody, bool instruct);
ast_ac_t* parser_parse_annotation();
ast_namespace_t* parser_parse_namespace(ast_body_t* ans);
ast_struct_t* parser_parse_struct(ast_body_t* ans);
ast_function_t* parser_parse_function(ast_body_t* ans);
ast_variable_t* parser_parse_variable(ast_body_t* ans, bool inbody, bool global);
ast_body_t* parser_parse_body();
ast_expr_t* parser_parse_expr(bool bodyparse);
ast_type_t* parser_parse_type();

ast_ac_t* parser_parse_ac(bool annotation);
ast_variable_t* parser_parse_var_or_arg_define();

ast_context_t* parser_parse(token_t* token);

#endif /* __PARSER_H__ */