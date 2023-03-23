#include <print.h>
#include <builder.h>

#include <stdio.h>
#include <stddef.h>

void builder_build_context(ast_context_t* context) {
    for (uint8_t i = 0; i < context->func; i++) {
        builder_build_function(context->funs[i]);
    }
}

void builder_build_function(ast_function_t* function) {
    builder_build_body_cycle(function->body);
}

bool builder_build_body_cycle(ast_body_t* body) {
    uint32_t counter = 0;
    for (uint8_t i = 255; i; i--) {
        while (builder_build_body(body, i)) {
            counter++;
        }
    }
    return counter > 0;
}

bool builder_build_body(ast_body_t* body, uint8_t priority) {
    ast_expr_t* last = body->exprs;
    while (last != NULL) {
        // Сравниваем приоритет
        if (builder_priority(last) == priority) {
            switch (last->type) {
                // Собираем выражения в "телах"
                case AST_BODY:
                    if (builder_build_body_cycle((void*) last))
                        return true;
                    goto step;
                case AST_CALL:
                    if (builder_build_body_cycle(((ast_call_t*) last)->args))
                        return true;
                    goto step;
                // Собираем математические выражения
                case AST_MATH: {
                    ast_math_t* math = (ast_math_t*) last;
                    // Проверяем собрано ли выражение
                    if (math->left || math->right)
                        goto step; // Если да, то перебираем дальше
                    // Собираем выражение (слево)
                    if (math->operation != MOP_NOT) {
                        ast_expr_t* pp = (math->left = last->prev)->prev;
                        ast_set_prev(last, pp);
                        if (!pp) body->exprs = last;
                    }
                    // Собираем выражение (справа)
                    ast_set_next(last, (math->right = last->next)->next);
                    // Успешный выход
                    return true;
                }
                default:
                    break;
            }
        }
        // Перебираем выражения
        step: {
            last = last->next;
        }
    }
    return false;
}

uint8_t builder_priority(ast_expr_t* expression) {
    switch (expression->type) {
        case AST_BODY:
            return 255;
        case AST_CALL:
            return 254;
        case AST_MATH: {
            ast_math_t* math = (void*) expression;
            switch (math->operation) {
                case MOP_NOT:
                    return 5;
                case MOP_AND:
                case MOP_OR:
                case MOP_XOR:
                    return 4;
                case MOP_ADD:
                case MOP_SUB:
                    return 2;
                case MOP_MUL:
                case MOP_DIV:
                    return 3;
                case MOP_DEREFERENCE:
                    return 254;
                case MOP_ASSIGN:
                    return 1;
            }
        }
        case AST_NUMBER:
        case AST_CHAR:
        case AST_STRING:
        case AST_NAMING:
            return 0;
        // todo:
        default:
            return 0;
    }
}