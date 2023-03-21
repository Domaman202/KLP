#include <ast.h>
#include <util.h>

#include <stdlib.h>
#include <string.h>

ast_context_t* ast_context_allocate() {
    ast_context_t* context = calloc(1, sizeof(ast_context_t));
    context->expr.type = AST_CONTEXT;
    return context;
}

ast_function_t* ast_function_allocate() {
    ast_function_t* function = calloc(1, sizeof(ast_function_t));
    function->expr.type = AST_FUNCTION;
    function->body = ast_body_allocate();
    return function;
}

ast_body_t* ast_body_allocate() {
    ast_body_t* body = calloc(1, sizeof(ast_body_t));
    body->expr.type = AST_BODY;
    return body;
}

ast_variable_t* ast_variable_allocate() {
    ast_variable_t* variable = calloc(1, sizeof(ast_variable_t));
    variable->expr.type = AST_VARIABLE;
    return variable;
}

ast_math_t* ast_math_allocate(ast_math_oper_t operation) {
    ast_math_t* math = calloc(1, sizeof(ast_math_t));
    math->expr.type = AST_MATH;
    math->operation = operation;
    return math;
}

ast_value_t* ast_value_allocate(ast_expr_type_t type) {
    ast_value_t* naming = calloc(1, sizeof(ast_value_t));
    naming->expr.type = type;
    return naming;
}

ast_type_t* ast_type_allocate() {
    ast_type_t* type = calloc(1, sizeof(ast_type_t));
    type->expr.type = AST_TYPE;
    return type;
}

void ast_body_add(ast_body_t* body, ast_expr_t* expr) {
    ast_expr_t** ptr = &body->exprs;
    while (*ptr)
        ptr = &(*ptr)->next;
    *ptr = expr;
}