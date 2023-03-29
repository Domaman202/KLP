#ifndef __UNROLLER_H__
#define __UNROLLER_H__

#include <ast.h>

void unroller_unroll_context(ast_context_t* context);
void unroller_unroll_function(ast_function_t* function);
ast_body_t* unroller_unroll_body(ast_body_t* body, ast_body_t* nbody);
ast_expr_t* unroller_unroll_expression(ast_body_t* nbody, ast_expr_t* expression);
ast_expr_t* unroller_unroll_argument(ast_body_t* nbody, ast_expr_t* expression);
ast_expr_t* unroller_save_tmp(ast_body_t* nbody, ast_expr_t* expression);

#endif /* __UNROLLER_H__ */