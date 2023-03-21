#ifndef __SS_H__
#define __SS_H__

#include <ast.h>

extern uint32_t ss_tmp;

void ss_ss_context(ast_context_t* context);
void ss_ss_function(ast_function_t* function);
ast_expr_t* ss_ss_expression(ast_body_t* body, ast_expr_t* expression);

uint8_t ss_priority(ast_expr_t* expression);
void ss_addsort(ast_body_t* body, ast_expr_t* expr);

#endif /* __S_H__ */