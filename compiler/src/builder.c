#include <error.h>
#include <print.h>
#include <builder.h>

#include <stdio.h>
#include <stddef.h>

uint32_t builder_tmp;

ast_expr_t* builder_build_body_cycle(ast_body_t* body, ast_body_t* actions) {
    // body->exprs = actions->exprs;
    ///
    ast_expr_t* result = NULL;
    for (uint8_t i = BUILDER_PG_A; i > BUILDER_PG_NB; i--) {
        ast_expr_t* tmp = builder_build_body(body, actions, i);
        if (tmp) {
            result = tmp;
        }
    }
    //
    return result;
}

ast_expr_t* builder_build_body(ast_body_t* body, ast_body_t* actions, uint8_t priority) {
    ast_expr_t* result = NULL;
    // Собираем выражения
    ast_expr_t* last = actions->exprs;
    while (last != NULL) {
        // Сравниваем приоритет
        if (builder_priority(last) == priority) { // Если приоритет совпал
            // DEBUG
            printf("\nDO:\n");
            ast_expr_print(0, (void*) actions);
            // Собираем выражения
            ast_expr_t* tmp = builder_build_expression(body, actions, last, false);
            if (tmp) {
                result = tmp;
            }
            // DEBUG
            printf("\nPOSLE:\n");
            ast_expr_print(0, (void*) actions);
        }
        // Перебираем выражения
        last = last->next;
    }
    // Выход
    return result;
}

ast_expr_t* builder_build_expression(ast_body_t* body, ast_body_t* actions, ast_expr_t* last, bool val_parse) {
    switch (last->type) {
        // // Присваиваем аннотацию
        // case AST_ANNOTATION:
        //     // Выпиливаем аннотацию из списка выражений
        //     if (!last->prev) actions->exprs = last->next;
        //     ast_set_prev(last->next, last->prev);
        //     // Присваиваем аннотацию переднему выражению
        //     ast_add_annotation(last->next, (void*) last);
        //     last->next = NULL;
        //     // Собираем аргументы
        // case AST_CALL: {
        //     ast_call_t* call = (void*) last;
        //     // Собираем адрес
        //     ast_expr_t* pp = (call->address = last->prev)->prev;
        //     ast_set_prev(last, pp);
        //     if (!pp) actions->exprs = last;
        //     // Собираем аргументы
        //     if (builder_build_body_cycle(body, call->args))
        //         return true;
        //     break;
        // }
        // Собираем выражения в "телах"
        case AST_BODY: {
            // Собираем выражение
            ast_expr_t* result = builder_build_body_cycle(body, (void*) last);
            // Если мы парсим значение, а не просто тело
            // if (val_parse) { // Если мы возвращаем значение
                // Сохраняем значение последнего действия в теле
                result = builder_save_tmp(body, result, MOP_ASSIGN);
                // Заменяем выражение
                ast_insert(result, last->prev, last->next, actions);
            // } else {
            //     // Вырезаем выражение
            //     ast_cute(last->prev, last->next, actions);
            // }
            // Выход
            return result;
        }
        // Собираем математические выражения
        case AST_MATH: {
            // Собираем выражение
            ast_math_t* math = (void*) last;
            ast_math_t* nmath = ast_math_allocate(math->operation);
            if (math->operation == MOP_NOT) { // Проверка навыражение с одним аргументом
                // todo: Собираем выражение с одним аргументом
                throw_invalid_ast((void*) math);
            } else {
                // Собираем аргументы
                // Собираем левый аргумент
                if (math->operation == MOP_ASSIGN)
                    nmath->left = last->prev;
                else nmath->left = builder_get_argument(body, actions, last->prev, math->operation == MOP_DEREFERENCE_SET || math->operation == MOP_DEREFERENCE_GET);
                // Собираем правый аргумент
                nmath->right = builder_get_argument(body, actions, last->next, false);
                // Создаём tmp
                ast_expr_t* tmp = builder_save_tmp(body, (void*) nmath, MOP_ASSIGN);
                // void* tmp = nmath;
                // Заменяем выражение и аргументы
                ast_insert(tmp, last->prev->prev, last->next->next, actions);
                // Выход
                return tmp;
            }
        }
        // Собираем условие
        case AST_IF: {
            // Собираем выражение
            ast_if_t* if_ = (void*) last;
            ast_if_t* nif = ast_if_allocate();
            // Вырезаем выражение
            ast_cute(last->prev, last->next, actions);
            // Собираем условие
            nif->condition = builder_build_expression(body, actions, if_->condition, true);
            // Собираем действие
            void* act = ast_body_allocate();
            builder_build_expression(act, actions, if_->action, false);
            nif->action = act;
            // Собираем "иначе"
            if (if_->else_action) {
                void* eact = ast_body_allocate();
                builder_build_expression(eact, actions, if_->else_action, false);
                nif->else_action = eact;
            }
            // Добавляем выражение в тело
            ast_body_add(body, (void*) nif);
            // Выход
            return NULL; // todo: i = if {i > 5} then {i} else {5}
        }
        // Собираем цикл
        case AST_WHILE: {
            // Собираем выражение
            ast_while_t* while_ = (void*) last;
            ast_while_t* nwhile = ast_while_allocate();
            // Вырезаем выражение
            ast_cute(last->prev, last->next, actions);
            // Собираем условие
            void* condition = ast_body_allocate();
            builder_build_expression(condition, actions, while_->condition, false);
            nwhile->condition = condition;
            // Собираем действие
            void* act = ast_body_allocate();
            builder_build_expression(act, actions, while_->action, false);
            nwhile->action = act;
            // Добавляем выражение в тело
            ast_body_add(body, (void*) nwhile);
            // Выход
            return NULL;
        }
        // Собираем выражения возврата
        case AST_RETURN: {
            // Собираем выражение
            ast_expr_t* value = builder_build_expression(body, actions, ((ast_return_t*) last)->value, true);
            if (val_parse) { 
                // Возвращаем наше значение
                ast_insert(value, last->prev, last->next, actions);
                // Выход
                return value;
            } else {
                // Вырезаем выражение и значение
                ast_cute(last->prev, last->next, actions);
                // Добавляем выражение в тело
                ast_return_t* ret = ast_return_allocate();
                ret->value = value;
                ast_body_add(body, (void*) ret);
                // Выход
                return NULL; 
            }
        }
        
        default:
            // return NULL;
            throw_invalid_ast(last);
    }
}

ast_expr_t* builder_get_argument(ast_body_t* body, ast_body_t* actions, ast_expr_t* last, bool dereference) {
    switch (last->type) {
        // Значения
        case AST_EMPTY:
        case AST_NUMBER:
        case AST_CHAR:
        case AST_STRING:
        case AST_TMP:
            return last;
        // Собираем наименование
        case AST_NAMING: {
            // Проверка на получение адреса для разыменовывания 
            if (dereference) {
                // Если это получение адреса для разыменовывания
                return last;
            } else {
                // Вырезаем выражение
                ast_cute(last->prev, last->next, actions);
                // Сохраняем в tmp
                void* value = ast_value_allocate(AST_NAMING, ((ast_value_t*) last)->value);
                ast_expr_t* tmp = builder_save_tmp(body, value, MOP_REFERENCE);
                // Заменяем выражение
                ast_insert(tmp, last->prev, last->next, actions);
                // Выход
                return tmp;
            }
        }
        default:
            throw_invalid_ast(last);
    }
}

ast_expr_t* builder_save_tmp(ast_body_t* body, ast_expr_t* expression, ast_math_oper_t operation) {
    if (expression->type == AST_TMP)
        return expression;
    ast_expr_t* ltmp = (void*) ast_value_allocate(AST_TMP, builder_tmp++);
    ast_math_t* lset = ast_math_allocate(operation);
    lset->left = ltmp;
    lset->right = expression;
    ast_body_add(body, (void*) lset);
    return ltmp;
}

uint8_t builder_priority(ast_expr_t* expression) {
    switch (expression->type) {
        case AST_EMPTY:
        case AST_NUMBER:
        case AST_CHAR:
        case AST_STRING:
        case AST_NAMING:
            return BUILDER_PG_NB;
        case AST_IF:
        case AST_WHILE:
            return BUILDER_PG_10;
        case AST_MATH: {
            ast_math_t* math = (void*) expression;
            switch (math->operation) {
                case MOP_REFERENCE:
                case MOP_DEREFERENCE_GET:
                    return BUILDER_PG_12;
                case MOP_ASSIGN:
                    return BUILDER_PG_11;
                case MOP_DEREFERENCE_SET:
                    return BUILDER_PG_10;
                case MOP_OR:
                    return BUILDER_PG_8;
                case MOP_XOR:
                    return BUILDER_PG_7;
                case MOP_AND:
                    return BUILDER_PG_6;
                case MOP_EQ:
                case MOP_NEQ:
                    return BUILDER_PG_5;
                case MOP_GREAT:
                case MOP_GOE:
                case MOP_LESS:
                case MOP_LOE:
                    return BUILDER_PG_4;
                case MOP_RIGHT_SHIFT:
                case MOP_LEFT_SHIFT:
                    return BUILDER_PG_3;
                case MOP_ADD:
                case MOP_SUB:
                    return BUILDER_PG_2;
                case MOP_MUL:
                case MOP_DIV:
                    return BUILDER_PG_1;
                case MOP_NOT:
                    return BUILDER_PG_0;
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