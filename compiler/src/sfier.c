#include <sfier.h>
#include <print.h>

#include <stddef.h>

void sfier_simplify_context(ast_context_t* context) {
    for (uint8_t i = 0; i < context->func; i++)
        sfier_simplify_function(context->funs[i]);
    for (uint8_t i = 0; i < context->nsc; i++) {
        sfier_simplify_context((void*) context->nss[i]);
    }
}

void sfier_simplify_function(ast_function_t* function) {
    function->body->exprs = sfier_simplify_expression(function->body->exprs);
}

ast_expr_t* sfier_simplify_expression(ast_expr_t* expr) {
    if (expr) {
        switch (expr->type) {
            case AST_CALL: {
                ast_call_t* call = (void*) expr;
                call->address = sfier_simplify_expression(call->address);
            }
            case AST_ANNOTATION: {
                ast_ac_t* ac = (void*) expr;
                ac->args = (void*) sfier_simplify_expression((void*) ac->args);
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
            case AST_RETURN: {
                ast_return_t* ret = (void*) expr;
                ret->value = sfier_simplify_expression(ret->value);
                break;
            }
            default:
                break;
        }
        return expr;
    } else return NULL;
}