#include <ss.h>

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

uint32_t ss_tmp;

void ss_ss_context(ast_context_t* context) {
    for (uint8_t i = 0; i < context->func; i++) {
        ss_ss_function(context->funs[i]);
    }
}

void ss_ss_function(ast_function_t* function) {
    ast_expr_t** last = &function->body->exprs;
    while (*last) {
        *last = (ast_expr_t*) ss_ss_expression(NULL, *last);
        last = &(*last)->next;
    }
}

ast_expr_t* ss_ss_expression(ast_body_t* body, ast_expr_t* expression) {
    if (expression == NULL)
        return NULL;
    switch (expression->type) {
        case AST_NUMBER:
        case AST_CHAR:
        case AST_STRING:
        case AST_NAMING:
            return expression;
        case AST_MATH: {
            bool first = body == NULL;
            if (first) body = ast_body_allocate();
            // Разбор математического выражения
            ast_math_t* math = (ast_math_t*) expression;
            math->left = ss_ss_expression(body, math->left);
            math->right = ss_ss_expression(body, math->right);
            // Если это начальное выражение - добавляем выражение и возвращаем тело
            if (first) {
                ss_addsort(body, expression);
                return (ast_expr_t*) body;
            }
            // Если это подвыражение - создаём и возвращаем tmp переменную с его результатом
            // Выделяем память
            ast_value_t* tmp = ast_value_allocate(AST_NAMING);
            ast_math_t* tmp_set = ast_math_allocate(MOP_ASSIGN);
            // Записываем id tmp переменной
            int length = snprintf(NULL, 0, "%d", ++ss_tmp);
            char* text = malloc(length + 1);
            snprintf(text, length + 1, "%d", ss_tmp);
            tmp->text = text;
            // Создаём присваивание tmp переменной
            tmp_set->left = (ast_expr_t*) tmp;
            tmp_set->right = (ast_expr_t*) math;
            ast_body_add(body, (ast_expr_t*) tmp_set);
            //
            return (ast_expr_t*) tmp;
        }
        default:
            return NULL;
    }
}

uint8_t ss_priority(ast_expr_t* expression) {
    return 0;
}

void ss_addsort(ast_body_t* body, ast_expr_t* expr) {
    ast_expr_t** ptr = &body->exprs;
    while (*ptr) {
        if (ss_priority(*ptr) < ss_priority(expr)) {
            ast_expr_t* next = *ptr;
            *ptr = expr;
            expr->next = next;
            return;
        } else {
            ptr = &(*ptr)->next;
        }
    }
    *ptr = expr;
}