#ifndef __SFIER__
#define __SFIER__

#include <ast.h>

void cleaner_clean_context(ast_context_t* context);
void cleaner_clean_function(ast_function_t* function);
ast_expr_t* cleaner_clean_expression(ast_expr_t* expr);

#endif /* __SFIER__ */