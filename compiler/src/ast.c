#include <ast.h>
#include <util.h>

#include <stdlib.h>
#include <string.h>

ast_expr_t* ast_empty_allocate() {
    ast_expr_t* empty = calloc(1, sizeof(ast_expr_t));
    empty->type = AST_EMPTY;
    return empty;
}

ast_context_t* ast_context_allocate() {
    ast_context_t* context = calloc(1, sizeof(ast_context_t));
    context->expr.type = AST_CONTEXT;
    return context;
}

ast_ac_t* ast_ac_allocate(ast_expr_type_t type, void* argument) {
    ast_ac_t* ac = calloc(1, sizeof(ast_ac_t));
    ac->expr.type = type;
    ac->argument = argument;
    ac->args = ast_body_allocate();
    return ac;
}

ast_namespace_t* ast_namespace_allocate(char* name) {
    ast_namespace_t* namespace = calloc(1, sizeof(ast_namespace_t));
    namespace->ctx.expr.type = AST_NAMESPACE;
    namespace->name = name;
    return namespace;
}

ast_struct_t* ast_struct_allocate() {
    ast_struct_t* structure = calloc(1, sizeof(ast_struct_t));
    structure->expr.type = AST_STRUCT;
    return structure;
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

ast_if_t* ast_if_allocate() {
    ast_if_t* if_ = calloc(1, sizeof(ast_if_t));
    if_->expr.type = AST_IF;
    return if_;
}

ast_return_t* ast_return_allocate() {
    ast_return_t* ret = calloc(1, sizeof(ast_return_t));
    ret->expr.type = AST_RETURN;
    return ret;
}

ast_value_t* ast_value_allocate(ast_expr_type_t type, uintptr_t val) {
    ast_value_t* value = calloc(1, sizeof(ast_value_t));
    value->expr.type = type;
    value->value = val;
    return value;
}

ast_type_t* ast_type_allocate() {
    ast_type_t* type = calloc(1, sizeof(ast_type_t));
    type->expr.type = AST_TYPE;
    return type;
}

void ast_cute(ast_expr_t* prev, ast_expr_t* next, ast_body_t* body) {
    if (prev) {
        ast_set_next(prev, next);
    } else {
        body->exprs = next;
    }
}

void ast_insert(ast_expr_t* expr, ast_expr_t* prev, ast_expr_t* next, ast_body_t* body) {
    if (prev)
        ast_set_next(prev, expr);
    else {
        body->exprs = expr;
        expr->prev = NULL;
    }

    if (next) {
        ast_set_prev(next, expr);
    } else {
        expr->next = NULL;
    }
}

void ast_set_next(ast_expr_t* expr, ast_expr_t* next) {
    expr->next = next;
    if (next) next->prev = expr;
}

void ast_set_prev(ast_expr_t* expr, ast_expr_t* prev) {
    expr->prev = prev;
    if (prev) prev->next = expr; 
}

void ast_add_annotation(ast_expr_t* expr, ast_ac_t* annotation) {
    if (!expr->annotations)
        expr->annotations = ast_body_allocate();
    ast_body_add(expr->annotations, (void*) annotation);
}

void ast_body_add(ast_body_t* body, ast_expr_t* expr) {
    if (body->exprs) {
        ast_expr_t* last = body->exprs;
        while (last->next != NULL)
            last = last->next;
        last->next = expr;
        expr->prev = last;
    } else {
        body->exprs = expr;
        expr->prev = NULL;
    }
}

void ast_body_addback(ast_body_t* body, ast_expr_t* expr) {
    expr->prev = NULL;
    expr->next = body->exprs;
    body->exprs = expr;
}