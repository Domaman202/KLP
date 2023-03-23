#include <ast.h>
#include <util.h>
#include <lexer.h>
#include <parser.h>

#include <stdlib.h>
#include <string.h>

// todo: debug
#include <stdio.h>

parser_t* parser;

void parser_throw(token_t* token) {
    parser->error = token;
    longjmp(parser->catch, 1);
}

bool parser_skip_space() {
    token_t* token = parser->token; 
    while (token->type == TK_SPACE)
        token = token->next;
    bool r = parser->token == token;
    parser->token = token;
    return r;
}

bool parser_skip_nl() {
    token_t* token = parser->token; 
    while (token->type == TK_NEWLINE)
        token = token->next;
    bool r = parser->token == token;
    parser->token = token;
    return r;
}

bool parser_skip() {
    token_t* token = parser->token; 
    while (token->type == TK_SPACE || token->type == TK_NEWLINE)
        token = token->next;
    bool r = parser->token == token;
    parser->token = token;
    return r;
}

void parser_prev() {
    if (parser->token->prev != NULL) {
        parser->token = parser->token->prev;
    }
}

token_t* parser_next() {
    parser_skip_space();
    token_t* token = parser->token;
    if (token->next != NULL)
        parser->token = token->next;
    return token;
}

token_t* parser_cnext(unsigned int argc, ...) {
    va_list types;
    va_start(types, argc);
    token_t* token = parser_next();
    for (; argc > 0; argc--) {
        token_type_t type = va_arg(types, token_type_t);
        if (token->type == type) {
            return token;
        }
    }
    parser_throw(token);
}

ast_function_t* parser_parse_function() {
    token_t* token = parser_next();
    if (token->type == TK_NAMING) {
        if (util_token_cmpfree(token, "fun")) {
            // Парсинг функции
            ast_function_t* function = ast_function_allocate();
            // Сохраняем имя
            function->name = token_text(parser_cnext(1, TK_NAMING));
            // Расчёт аргументов
            parser_cnext(1, TK_OPEN_BRACKET);
            // Аргументы
            while (1) {
                token_t* token = parser_cnext(3, TK_NAMING, TK_COMMA, TK_CLOSE_BRACKET);
                if (token->type == TK_NAMING) {
                    parser_prev();
                    function->args = (void*) util_reallocadd((void*) function->args, (void*) parser_parse_var_or_arg_define(), ++function->argc);
                } else if (token->type == TK_COMMA) {
                    continue;
                } else {
                    break;
                }
            }
            // Тип возврата
            parser_cnext(1, TK_COLON);
            function->rettype = parser_parse_type();
            // Тело функции
            parser_cnext(1, TK_ASSIGN);
            // Проверка на external
            token_t* token = parser_next();
            if (token->type == TK_NAMING) {
                if (util_token_cmpfree(token, "ext")) {
                    function->external = true;
                } else {
                    // Бросаем ошибку
                    parser_throw(token);
                }
            } else {
                parser_prev();
                parser_cnext(1, TK_OPEN_FIGURAL_BRACKET);
                function->body = parser_parse_body();
            }
            // Выход
            return function;
        } else parser_prev();
    } else parser_prev();
    return NULL;
}

ast_variable_t* parser_parse_variable(bool global) {
    token_t* token = parser_next();
    if (token->type == TK_NAMING) {
        if (util_token_cmpfree(token, "var")) {
            // Парсинг переменной
            ast_variable_t* variable;
            // Определние типа
            token = parser_cnext(1, TK_NAMING);
            bool type_defined = parser_cnext(2, TK_COLON, TK_ASSIGN)->type == TK_COLON;
            //
            if (type_defined) {
                parser->token = token;
                variable = parser_parse_var_or_arg_define();
            } else {
                variable = ast_variable_allocate();
                variable->name = token_text(token);
                parser_prev();
            }
            //
            variable->global = global;
            // Проверка на external и присваивание
            token_t* token_assign = parser_next();
            if (token_assign->type == TK_ASSIGN) {
                token_t* token_value = parser_cnext(1, TK_NAMING);
                if (util_token_cmpfree(token_value, "ext")) {
                    if (global) {
                        variable->external = true;
                    } else {
                        // Бросаем ошибку (локальная переменная не может быть external)
                        parser_throw(token_value);
                    }
                } else {
                    if (!global) {
                        parser_prev();
                        variable->assign = (ast_expr_t*) parser_parse_expr();
                    } else {
                        // Бросаем ошибку (переменной нельзя присвоить значение)
                        parser_throw(token_assign);
                    }
                }
            } else parser_prev();
            // Выход
            return variable;
        } else parser_prev();
    } else parser_prev();
    return NULL;
}

ast_body_t* parser_parse_body() {
    // Инициализация
    ast_body_t* body = ast_body_allocate();
    //
    while (1) {
        ast_body_t* expr = parser_parse_expr();
        if (expr->exprs) {
            ast_body_add(body, (ast_expr_t*) expr);
        }
        //
        token_t* token = parser_next();
        switch (token->type) {
            case TK_CLOSE_BRACKET:
            case TK_CLOSE_CUBE_BRACKET:
            case TK_CLOSE_FIGURAL_BRACKET:
                // Выход
                return body;
            case TK_SEMICOLON:
            case TK_NEWLINE:
                // Парсим следующее выражение
                break;
            default:
                // Кидаем исключение
                parser_throw(token);
        }
    }
}

ast_body_t* parser_parse_expr() {
    // Инициализация
    ast_body_t* body = ast_body_allocate();
    // Перебираем все токены
    while (1) {
        token_t* token = parser_next();
        switch (token->type) {
            case TK_OPEN_BRACKET:
                ast_body_add(body, (ast_expr_t*) parser_parse_body());
                break;
            case TK_CLOSE_BRACKET:
            case TK_CLOSE_CUBE_BRACKET:
            case TK_CLOSE_FIGURAL_BRACKET:
            case TK_COMMA:
            case TK_SEMICOLON:
            case TK_NEWLINE:
                parser_prev();
                return body;
            case TK_OPEN_CUBE_BRACKET:
                // Парсинг указателя/массива
                ast_body_add(body, (void*) parser_parse_expr());
                // Добавляем операцию
                ast_body_add(body, (void*) ast_math_allocate(MOP_DEREFERENCE));
                // Парсинг сдвига/индекса
                token = parser_next();
                if (token->type == TK_COMMA) {
                    // если есть запятая - есть сдвиг/индекс
                    ast_body_add(body, (void*) parser_parse_expr());
                    parser_cnext(1, TK_CLOSE_CUBE_BRACKET);
                } else if (token->type == TK_CLOSE_CUBE_BRACKET) {
                    // если скобка закрывается - разыминовывание
                    ast_body_add(body, ast_empty_allocate());
                    break;
                } else {
                    // Иначе кидаем ошибку
                    parser_throw(token);
                }
            case TK_NAMING: {
                // Проверка на вызов функции
                token_t* ntoken = parser_next();
                if (ntoken->type == TK_OPEN_BRACKET) {
                    // Парсим вызов функции
                    ast_call_t* call = ast_call_allocate(token_text(token));
                    ast_body_add(body, (void*) call);
                    // Парсинг аргументов
                    while (1) {
                        ast_body_add(call->args, (void*) parser_parse_expr());
                        // Парсинг аргументов
                        token = parser_next();
                        if (token->type == TK_COMMA) { // если есть запятая - парсим следующий аргумент
                            continue;
                        } else if (token->type == TK_CLOSE_BRACKET) { // если закрывается скобка - конец выражения вызова функции
                            break;
                        } else {
                            // Иначе кидаем ошибку
                            parser_throw(token);
                        }
                    }
                    break;
                } else parser_prev();
                // Парсим наименование
            }
            case TK_NUMBER:
            case TK_CHAR:
            case TK_STRING:
                ast_body_add(body, (void*) ast_value_allocate((ast_expr_type_t) token->type, token_text(token)));
                break;
            case TK_PLUS:
            case TK_MINUS:
            case TK_STAR:
            case TK_ASSIGN:
                ast_body_add(body, (void*) ast_math_allocate((ast_math_oper_t) token->type));
                break;
            default:
                parser_throw(token);
        }
    }
}

ast_type_t* parser_parse_type() {
    ast_type_t* type = ast_type_allocate();
    type->name = token_text(parser_cnext(1, TK_NAMING));
    token_t* token = parser_next();
    if (token->type == TK_LESS) {
        type->gen = parser_parse_type();
        parser_cnext(1, TK_GREAT);
    } else parser_prev();
    return type;
}

ast_variable_t* parser_parse_var_or_arg_define() {
    ast_variable_t* variable = ast_variable_allocate();
    variable->name = token_text(parser_cnext(1, TK_NAMING));
    parser_cnext(1, TK_COLON);
    variable->type = parser_parse_type();
    return variable;
}

parser_parse_result_t parser_parse(token_t* token) {
    // Инициализация
    parser = malloc(sizeof(parser_t));
    parser->token = token;
    parser->error = NULL;
    // Создаём контекст
    ast_context_t* context = ast_context_allocate();
    parser->context = context;
    // Установка обработчика ошибок
    if (setjmp(parser->catch) == 0) {
        // Проходимся по токенам
        while (parser->token->type != TK_EOF) {
            // Пропускаем бесполезные токены
            parser_skip();
            // Парсим переменную
            ast_variable_t* variable = parser_parse_variable(true);
            if (variable != NULL) {
                context->vars = (void*) util_reallocadd((void*) context->vars, (void*) variable, ++context->varc);
            }
            // Парсим функцию
            ast_function_t* function = parser_parse_function();
            if (function != NULL) {
                context->funs = (void*) util_reallocadd((void*) context->funs, (void*) function, ++context->func);
            }
        }
    }
    // Высвобождаем память
    token_t* error = parser->error;
    free(parser);
    parser = NULL;
    // Выход
    parser_parse_result_t result = { context, error };
    return result;
}