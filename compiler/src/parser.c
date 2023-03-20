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
                function->body = parser_parse_body(TK_OPEN_FIGURAL_BRACKET, TK_CLOSE_FIGURAL_BRACKET);
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
                        variable->assign = (ast_expr_t*) parser_parse_expr(NULL);
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

ast_body_t* parser_parse_body(token_type_t open, token_type_t close) {
    // Инициализация
    ast_body_t* body = ast_body_allocate();
    body->exprc = 0;
    body->exprs = malloc(0);
    ast_expr_t* left = NULL;
    // Пропускаем бесполезные токены
    parser_skip();
    // Проверяем наличие начала тела
    parser_cnext(1, open);
    // Пропускаем бесполезные токены
    parser_skip();
    // Обработка тела
    while (1) {
        token_t* token = parser_next();
        // Проверка на конец выражения
        if (token->type == close) {
            if (left != NULL) {
                ast_body_add(body, left);
                left = NULL;
            }
            break;
        } else {
            switch (token->type) {
                // При переносе строки сохраняем выражение
                case TK_NEWLINE:
                    ast_body_add(body, left);
                    left = NULL;
                    break;
                // При конце файла кидаем ошибки (тело не было закрыто)
                case TK_EOF:
                    parser_throw(token);
                // Проверяем на определение переменной
                case TK_NAMING:
                    if (left == NULL && util_token_cmpfree(token, "var")) {
                        parser_prev();
                        left = (void*) parser_parse_variable(false);
                        break;
                    }
                // Парсим выражение
                default:
                    parser_prev();
                    left = parser_parse_expr(left);
                    break;
            }
        }
    }
    // Выход
    return body;
}

ast_expr_t* parser_parse_expr(ast_expr_t* left) {
    while (1) {
        token_t* token = parser_next();
        printf("|0x%x\n", token->type);
        switch (token->type) {
            // Парсим выражение в скобках
            case TK_OPEN_BRACKET:
                parser_prev();
                left = (ast_expr_t*) parser_parse_body(TK_OPEN_BRACKET, TK_CLOSE_BRACKET);
                break;
            // Парсим разыминовывание указателя
            case TK_OPEN_CUBE_BRACKET: {
                ast_math_t* dereference = ast_math_allocate();
                dereference->operation = MOP_DEREFERENCE;
                dereference->left = parser_parse_expr(NULL);
                token = parser_next();
                if (token->type == TK_COMMA)
                    dereference->right = parser_parse_expr(NULL);
                else parser_prev();
                parser_cnext(1, TK_CLOSE_CUBE_BRACKET);
                left = (ast_expr_t*) dereference;
                break;
            }
            // Парсим мат. выражения и присваивание
            case TK_PLUS:
            case TK_MINUS:
            case TK_STAR:
            case TK_SLASH:
            case TK_ASSIGN: {
                ast_math_t* math = ast_math_allocate();
                math->operation = (ast_math_oper_t) token->type;
                math->left = left;
                math->right = parser_parse_expr(NULL);
                left = (ast_expr_t*) math;
                break;
            }
            // Парсим числа и названия
            case TK_NUMBER:
            case TK_CHAR:
            case TK_STRING:
            case TK_NAMING: {
                ast_value_t* value = ast_value_allocate((ast_expr_type_t) token->type);
                value->text = token_text(token);
                left = (ast_expr_t*) value;
                break;
            }
            // Конец выражения
            case TK_CLOSE_BRACKET:
            case TK_CLOSE_CUBE_BRACKET:
            case TK_CLOSE_FIGURAL_BRACKET:
            case TK_COMMA:
            case TK_NEWLINE:
                parser_prev();
                return left;
            // Иначе кидаем ошибку (неизвестный символ)
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