#ifndef __BUILDER_H__
#define __BUILDER_H__

#include <ast.h>

#include <stdbool.h>

void builder_build_context(ast_context_t* context);
void builder_build_function(ast_function_t* function);
bool builder_build_body_cycle(ast_body_t* body);
bool builder_build_body(ast_body_t* body, uint8_t priority);

uint8_t builder_priority(ast_expr_t* expression);

#endif /* __BUILDER_H__ */