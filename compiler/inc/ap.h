#ifndef __AP_H__
#define __AP_H__

#include <ast.h>

void ap_process_context(ast_context_t* context);
void ap_process_function(ast_context_t* context, ast_function_t* function);
void ap_process_expression(ast_context_t* context, ast_function_t* function, ast_body_t* body, ast_expr_t* expression);
void ap_process(ast_context_t* context, ast_function_t* function, ast_body_t* body, ast_expr_t* expression, ast_annotation_t* annotation);

#endif /* __AP_H__ */