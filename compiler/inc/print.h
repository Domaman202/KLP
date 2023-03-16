#ifndef __PRINT_H__
#define __PRINT_H__

#include <ast.h>
#include <lexer.h>

#include <stdbool.h>

void token_print(token_t* token, bool nss);

void ast_context_print(ast_context_t* context);
void ast_function_print(ast_function_t* function);
void ast_type_print(ast_type_t* type);

#endif /* __PRINT_H__ */