#ifndef __SFIER__
#define __SFIER__

#include <ast.h>

void sfier_simplify_context(ast_context_t* context);
void sfier_simplify_function(ast_function_t* function);
ast_expr_t* sfier_simplify_expression(ast_expr_t* expr);

#endif /* __SFIER__ */