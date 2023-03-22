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
                    if (builder_build_body_cycle((ast_body_t*) last))
                        return true;
                    goto step;
                // Собираем математические выражения
                case AST_MATH: {
                    ast_math_t* math = (ast_math_t*) last;
                    // Проверяем собрано ли выражение
                    if (math->left)
                        goto step; // Если да, то перебираем дальше
                    // Собираем выражение
                    math->left = last->prev;
                    math->right = last->next;
                    // Выпиливаем левое выражение
                    ast_expr_t* pp = last->prev->prev;
                    if (pp) pp->next = last;
                    else body->exprs = last;
                    last->prev = pp;
                    // Выпиливаем правое выражение
                    ast_expr_t* nn = (last->next = last->next->next);
                    if (nn) nn->prev = last;
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
        case AST_MATH: {
            ast_math_t* math = (ast_math_t*) expression;
            switch (math->operation) {
                case MOP_ADD:
                case MOP_SUB:
                    return 1;
                case MOP_MUL:
                case MOP_DIV:
                    return 2;
                case MOP_DEREFERENCE:
                    return 255;
                case MOP_ASSIGN:
                    return 254;
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