#include <print.h>
#include <builder.h>

#include <stdio.h>
#include <stddef.h>

void builder_build_context(ast_context_t* context) {
    for (uint8_t i = 0; i < context->func; i++)
        builder_build_function(context->funs[i]);
    for (uint8_t i = 0; i < context->nsc; i++) {
        builder_build_context((void*) context->nss[i]);
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
    // Собираем выражения
    ast_expr_t* last = body->exprs;
    while (last != NULL) {
        // Сравниваем приоритет
        if (builder_priority(last) == priority) {
            // Собираем выражение
            if (builder_build_expression(body, last)) { // Если выражение успешно собрано
                // Выходим
                return true;
            }
        }
        // Перебираем выражения
        last = last->next;
    }
    // Выход
    return false;
}

bool builder_build_expression(ast_body_t* body, ast_expr_t* last) {
    switch (last->type) {
        // Присваиваем аннотацию
        case AST_ANNOTATION:
            // Выпиливаем аннотацию из списка выражений
            if (!last->prev) body->exprs = last->next;
            ast_set_prev(last->next, last->prev);
            // Присваиваем аннотацию переднему выражению
            ast_add_annotation(last->next, (void*) last);
            last->next = NULL;
            // Собираем аргументы
        case AST_CALL: {
            ast_call_t* call = (void*) last;
            // Собираем адрес
            ast_expr_t* pp = (call->address = last->prev)->prev;
            ast_set_prev(last, pp);
            if (!pp) body->exprs = last;
            // Собираем аргументы
            if (builder_build_body_cycle(call->args))
                return true;
            break;
        }
        // Собираем выражения в "телах"
        case AST_BODY:
            if (builder_build_body_cycle((void*) last))
                return true;
            break;
        // Собираем математические выражения
        case AST_MATH: {
            ast_math_t* math = (ast_math_t*) last;
            //
            if (math->left || math->right) { // Проверяем собрано ли выражение
                // Перебираем выражения дальше
                break; 
            }
            // Собираем выражение (левое)
            if (math->operation != MOP_NOT) {
                ast_expr_t* pp = (math->left = last->prev)->prev;
                ast_set_prev(last, pp);
                if (!pp) body->exprs = last;
            }
            // Собираем выражение (правое)
            ast_set_next(last, (math->right = last->next)->next);
            // Успешный выход
            return true;
        }
        // Собираем выражения возврата
        case AST_RETURN:
            return builder_build_expression(body, ((ast_return_t*) last)->value);
        default:
            break;
    }
    // Перебираем выражения дальше
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
        case AST_BODY:
        case AST_CALL:
        case AST_RETURN:
            return BUILDER_PG_H;
        case AST_ANNOTATION:
            return BUILDER_PG_A;
        // todo:
        default:
            return BUILDER_PG_NB;
    }
}