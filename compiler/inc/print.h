#ifndef __PRINT_H__
#define __PRINT_H__

#include <ast.h>
#include <lexer.h>

#include <stddef.h>
#include <stdbool.h>

void token_print(token_t* token, bool nss);

void ast_expr_print(size_t indent, ast_expr_t* expression);
void ast_null_print(size_t indent);
void ast_empty_print(size_t indent);
void ast_context_print(size_t indent, ast_context_t* context);
void ast_function_print(size_t indent, ast_function_t* function);
void ast_variable_print(size_t indent, ast_variable_t* variable);
void ast_body_print(size_t indent, ast_body_t* body);
void ast_math_print(size_t indent, ast_math_t* math);
void ast_value_print(size_t indent, ast_value_t* naming);
void ast_type_print(ast_type_t* type);

#endif /* __PRINT_H__ */