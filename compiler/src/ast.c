#include <ast.h>

#include <stdlib.h>

ast_context_t* ast_context_allocate() {
    ast_context_t* context = malloc(sizeof(ast_context_t));
    context->expr.type = AST_CONTEXT;
    return context;
}

ast_function_t* ast_function_allocate() {
    ast_function_t* function = malloc(sizeof(ast_function_t));
    function->expr.type = AST_FUNCTION;
    function->body = ast_body_allocate();
    return function;
}

ast_body_t* ast_body_allocate() {
    ast_body_t* body = malloc(sizeof(ast_body_t));
    body->expr.type = AST_BODY;
    return body;
}

ast_variable_t* ast_variable_allocate() {
    ast_variable_t* variable = malloc(sizeof(ast_variable_t));
    variable->expr.type = AST_VARIABLE;
    return variable;
}

ast_math_t* ast_math_allocate() {
    ast_math_t* math = malloc(sizeof(ast_math_t));
    math->expr.type = AST_MATH;
    return math;
}

ast_naming_t* ast_naming_allocate() {
    ast_naming_t* naming = malloc(sizeof(ast_naming_t));
    naming->expr.type = AST_NAMING;
    return naming;
}

ast_type_t* ast_type_allocate() {
    ast_type_t* type = malloc(sizeof(ast_type_t));
    type->expr.type = AST_TYPE;
    return type;
}