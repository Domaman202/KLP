#include <error.h>
#include <cleaner.h>
#include <unroller.h>

#include <stdio.h>
#include <stddef.h>

uint32_t unroller_tmp;

void unroller_unroll_context(ast_context_t* context) {
    for (uint8_t i = 0; i < context->func; i++) {
        unroller_unroll_function(context->funs[i]);
    }
}

void unroller_unroll_function(ast_function_t* function) {
    function->body = unroller_unroll_body(function->body, ast_body_allocate());
}

ast_body_t* unroller_unroll_body(ast_body_t* body, ast_body_t* nbody) {
    // Перебираем выражения
    ast_expr_t* expression = body->exprs;
    while (expression) {
        ast_body_add(nbody, unroller_unroll_expression(nbody, expression));
        expression = expression->next;
    }
    // Выход
    return nbody;
}

ast_expr_t* unroller_unroll_expression(ast_body_t* nbody, ast_expr_t* expression) {
    switch (expression->type) {
        case AST_BODY: {
            ast_body_t* body = (void*) expression;
            ast_expr_t* expr = body->exprs;
            while (expr->next) {
                ast_body_add(nbody, unroller_unroll_expression(nbody, expr));
                expr = expr->next;
            }
            return unroller_unroll_expression(nbody, expr);
        }
        case AST_MATH: {
            ast_math_t* omath = (void*) expression;
            ast_math_t* nmath = ast_math_allocate(omath->operation);
            nmath->left = unroller_unroll_argument(nbody, omath->left);
            nmath->right = unroller_unroll_argument(nbody, omath->right);
            return (void*) nmath;
        }
        case AST_RETURN:
            return (void*) ast_return_allocate(unroller_unroll_argument(nbody, ((ast_return_t*) expression)->value));
        case AST_NAMING:
            return (void*) ast_value_allocate(AST_NAMING, ((ast_value_t*) expression)->value);
        default:
            throw_invalid_ast(expression);
    }
}

ast_expr_t* unroller_unroll_argument(ast_body_t* nbody, ast_expr_t* expression) {
    printf(">1\n");
    void* result = unroller_save_tmp(nbody, unroller_unroll_expression(nbody, expression));
    printf("<1\n");
    return result;
}

ast_expr_t* unroller_save_tmp(ast_body_t* nbody, ast_expr_t* expression) {
    ast_expr_t* ltmp = (void*) ast_value_allocate(AST_TMP, unroller_tmp++);
    ast_math_t* lset = ast_math_allocate(MOP_ASSIGN);
    lset->left = ltmp;
    lset->right = expression;
    ast_body_add(nbody, (void*) lset);
    return ltmp;
}