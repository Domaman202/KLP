#include <sfier.h>
#include <print.h>

#include <stddef.h>

void sfier_simplify_context(ast_context_t* context) {
    for (uint8_t i = 0; i < context->func; i++) {
        sfier_simplify_function(context->funs[i]);
    }
}

void sfier_simplify_function(ast_function_t* function) {
    function->body->exprs = sfier_simplify_expression(function->body->exprs);
}

ast_expr_t* sfier_simplify_expression(ast_expr_t* expr) {
    if (expr) {
        switch (expr->type) {
            case AST_ANNOTATION:
            case AST_CALL: {
                ast_expr_t** args = (void*) &((ast_ac_t*) expr)->args;
                *args = sfier_simplify_expression(*args);
                break;
            }
            case AST_BODY: {
                ast_body_t* body = (void*) expr;
                ast_expr_t** last = &body->exprs;
                while (*last) {
                    ast_expr_t* prev = (*last)->prev;
                    ast_expr_t* next = (*last)->next;
                    ast_expr_t* simplified = sfier_simplify_expression(*last);
                    ast_set_prev(simplified, prev);
                    ast_set_next(simplified, next);
                    *last = simplified;
                    last = &simplified->next;
                }
                if (body->exprs->next)
                    break;
                return body->exprs;
            }
            case AST_MATH: {
                ast_math_t* math = (void*) expr;
                math->left = sfier_simplify_expression(math->left);
                math->right = sfier_simplify_expression(math->right);
                break;
            }
            default:
                break;
        }
        return expr;
    } else return NULL;
}