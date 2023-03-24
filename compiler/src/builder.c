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
                // Собираем аргументы
                case AST_ANNOTATION:
                case AST_CALL:
                    if (builder_build_body_cycle(((ast_con_t*) last)->args))
                        return true;
                    goto step;
                // Собираем выражения в "телах"
                case AST_BODY:
                    if (builder_build_body_cycle((void*) last))
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
        case AST_NUMBER:
        case AST_CHAR:
        case AST_STRING:
        case AST_NAMING:
            return BUILDER_PG_NB;
        case AST_MATH: {
            ast_math_t* math = (void*) expression;
            switch (math->operation) {
                case MOP_ASSIGN:
                    return BUILDER_PG_L;
                case MOP_OR:
                    return BUILDER_PG_12;
                case MOP_XOR:
                    return BUILDER_PG_11;
                case MOP_AND:
                    return BUILDER_PG_10;
                case MOP_EQ:
                case MOP_NEQ:
                    return BUILDER_PG_9;
                case MOP_GREAT:
                case MOP_GOE:
                case MOP_LESS:
                case MOP_LOE:
                    return BUILDER_PG_8;
                case MOP_RIGHT_SHIFT:
                case MOP_LEFT_SHIFT:
                    return BUILDER_PG_7;
                case MOP_ADD:
                case MOP_SUB:
                    return BUILDER_PG_6;
                case MOP_MUL:
                case MOP_DIV:
                    return BUILDER_PG_5;
                case MOP_NOT:
                    return BUILDER_PG_3;
                case MOP_DEREFERENCE:
                    return BUILDER_PG_H;
            }
        }
        case AST_ANNOTATION:
        case AST_BODY:
        case AST_CALL:
            return BUILDER_PG_H;
        // todo:
        default:
            return BUILDER_PG_NB;
    }
}