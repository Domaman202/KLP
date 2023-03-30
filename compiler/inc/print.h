#ifndef __PRINT_H__
#define __PRINT_H__

#include <ast.h>
#include <lexer.h>

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

extern FILE* print_stream;

void print_init();

void token_print(token_t* token, bool nss);

void ast_expr_print(size_t indent, ast_expr_t* expression);
void ast_null_print(size_t indent);
void ast_empty_print(size_t indent);
void ast_context_print(size_t indent, ast_context_t* context);
void ast_ac_print(size_t indent, ast_ac_t* ac);
void ast_annotation_print(size_t indent, ast_expr_t* expression);
void ast_struct_print(size_t indent, ast_struct_t* structure);
void ast_function_print(size_t indent, ast_function_t* function);
void ast_variable_print(size_t indent, ast_variable_t* variable);
void ast_body_print(size_t indent, ast_body_t* body, char* text);
void ast_math_print(size_t indent, ast_math_t* math);
void ast_if_print(size_t indent, ast_if_t* if_);
void ast_while_print(size_t indent, ast_while_t* while_);
void ast_return_print(size_t indent, ast_return_t* ret);
void ast_value_print(size_t indent, ast_value_t* naming);
void ast_type_print(ast_type_t* type);

#endif /* __PRINT_H__ */