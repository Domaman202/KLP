#include <print.h>
#include <cleaner.h>

void cleaner_clean_context(ast_context_t* context) {
    for (uint8_t i = 0; i < context->func; i++) {
        cleaner_clean_function(context->funs[i]);
    }
}

void cleaner_clean_function(ast_function_t* function) {
    function->body->exprs = cleaner_clean_expression(function->body->exprs);
}

ast_expr_t* cleaner_clean_expression(ast_expr_t* expr) {
    if (expr) {
        switch (expr->type) {
            case AST_CALL: {
                ast_call_t* call = (void*) expr;
                call->address = cleaner_clean_expression(call->address);
            }
            case AST_ANNOTATION: {
                ast_ac_t* ac = (void*) expr;
                ac->args = (void*) cleaner_clean_expression((void*) ac->args);
                break;
            }
            case AST_BODY: {
                ast_body_t* body = (void*) expr;
                ast_expr_t** last = &body->exprs;
                if (last && *last) {
                    while (*last) {
                        ast_expr_t* prev = (*last)->prev;
                        ast_expr_t* next = (*last)->next;
                        ast_expr_t* simplified = cleaner_clean_expression(*last);
                        ast_set_prev(simplified, prev);
                        ast_set_next(simplified, next);
                        *last = simplified;
                        last = &simplified->next;
                    }
                    if (body->exprs->next) {
                        break;
                    }
                }
                return body->exprs;
            }
            case AST_MATH: {
                ast_math_t* math = (void*) expr;
                math->left = cleaner_clean_expression(math->left);
                math->right = cleaner_clean_expression(math->right);
                break;
            }
            case AST_RETURN: {
                ast_return_t* ret = (void*) expr;
                ret->value = cleaner_clean_expression(ret->value);
                break;
            }
            default:
                break;
        }
        return expr;
    } else return NULL;
}