#include <ast.h>
#include <lexer.h>
#include <parser.h>

#include <stdlib.h>
#include <string.h>

// todo: debug
#include <stdio.h>

void* parser_error;

bool parser_skip_space(parser_t* parser) {
    token_t* token = parser->token; 
    while (token->type == TK_SPACE)
        token = token->next;
    bool r = parser->token == token;
    parser->token = token;
    return r;
}

bool parser_skip_nl(parser_t* parser) {
    token_t* token = parser->token; 
    while (token->type == TK_NEWLINE)
        token = token->next;
    bool r = parser->token == token;
    parser->token = token;
    return r;
}

bool parser_skip(parser_t* parser) {
    token_t* token = parser->token; 
    while (token->type == TK_SPACE || token->type == TK_NEWLINE)
        token = token->next;
    bool r = parser->token == token;
    parser->token = token;
    return r;
}

void parser_prev(parser_t* parser) {
    if (parser->token->prev != NULL) {
        parser->token = parser->token->prev;
    }
}

token_t* parser_next(parser_t* parser) {
    parser_skip_space(parser);
    token_t* token = parser->token;
    if (token->next != NULL)
        parser->token = token->next;
    return token;
}

token_t* parser_cnext(parser_t* parser, jmp_buf catch, unsigned int argc, ...) {
    va_list types;
    va_start(types, argc);
    token_t* token = parser_next(parser);
    for (; argc > 0; argc--) {
        token_type_t type = va_arg(types, token_type_t);
        if (token->type == type) {
            return token;
        }
    }
    parser_error = token;
    longjmp(catch, 1);
}

ast_function_t* parser_parse_function(parser_t* parser, jmp_buf catch) {
    token_t* token = parser_next(parser);
    if (token->type == TK_NAMING) {
        char* text = token_text(token);
        if (!strcmp(text, "fun")) {
            // Парсинг функции
            ast_function_t* function = ast_function_allocate();
            // Сохраняем имя
            function->name = token_text(parser_cnext(parser, catch, 1, TK_NAMING));
            // Расчёт аргументов
            parser_cnext(parser, catch, 1, TK_OPEN_BRACKET);
            // Аргументы
            uint8_t argc = 0;
            ast_variable_t** args = NULL;
            while (1) {
                token_t* token = parser_cnext(parser, catch, 3, TK_NAMING, TK_COMMA, TK_CLOSE_BRACKET);
                if (token->type == TK_NAMING) {
                    parser_prev(parser);
                    size_t size = sizeof(ast_variable_t*) * ++argc;
                    void* tmp = malloc(size);
                    memcpy(tmp, args, size - sizeof(ast_variable_t*));
                    free(args);
                    args = tmp;
                    args[argc - 1] = parser_parse_variable_define(parser, catch);
                } else if (token->type == TK_COMMA)
                    continue;
                else break;
            }
            function->argc = argc;
            function->args = args;
            // Тип возврата
            parser_cnext(parser, catch, 1, TK_COLON);
            function->rettype = parser_parse_type(parser, catch);
            // Тело функции
            parser_cnext(parser, catch, 1, TK_ASSIGN);
            function->body = parser_parse_body(parser, catch, TK_OPEN_FIGURAL_BRACKET, TK_CLOSE_FIGURAL_BRACKET);
            // Выход
            free(text);
            return function;
        }
        free(text);
    }
    return NULL;
}

ast_body_t* parser_parse_body(parser_t* parser, jmp_buf catch, token_type_t open, token_type_t close) {
    // Инициализация
    ast_body_t* body = ast_body_allocate();
    body->exprc = 0;
    body->exprs = malloc(0);
    ast_expr_t* left;
    // Пропускаем бесполезные токены
    parser_skip(parser);
    // Проверяем наличие начала тела
    parser_cnext(parser, catch, 1, open);
    // Пропускаем бесполезные токены
    parser_skip(parser);
    // Обработка тела
    while (1) {
        // Пропускаем пробелы
        parser_skip_space(parser);
        //
        token_t* token = parser_next(parser);
        if (token->type == close) {
            ast_body_add(body, left);
            left = NULL;
            break;
        } else {
            switch (token->type) {
                case TK_NEWLINE:
                    ast_body_add(body, left);
                    left = NULL;
                    break;
                case TK_EOF:
                    parser_error = token;
                    longjmp(catch, 1);
                default:
                    parser_prev(parser);
                    left = parser_parse_expr(parser, catch, left);
                    break;
            }
        }
    }
    // Выход
    return body;
}

ast_expr_t* parser_parse_expr(parser_t* parser, jmp_buf catch, ast_expr_t* left) {
    token_t* token = parser_next(parser);
    switch (token->type) {
        case TK_OPEN_BRACKET:
            parser_prev(parser);
            return (ast_expr_t*) parser_parse_body(parser, catch, TK_OPEN_BRACKET, TK_CLOSE_BRACKET);
            break;
        case TK_PLUS:
        case TK_MINUS:
        case TK_STAR:
        case TK_SLASH: {
            ast_math_t* math = ast_math_allocate();
            math->operation = (ast_math_oper_t) token->type;
            math->left = left;
            math->right = parser_parse_expr(parser, catch, NULL);
            return (ast_expr_t*) math;
        }
        case TK_ASSIGN: {
            ast_math_t* math = ast_math_allocate();
            math->operation = MOP_ASSIGN;
            math->left = left;
            math->right = parser_parse_expr(parser, catch, NULL);
            return (ast_expr_t*) math;
        }
        case TK_NAMING: {
            ast_naming_t* naming = ast_naming_allocate();
            naming->name = token_text(token);
            return (ast_expr_t*) naming;
        }
        default: {
            parser_error = token;
            longjmp(catch, 1);
        }
    }
    return NULL;
}

ast_type_t* parser_parse_type(parser_t* parser, jmp_buf catch) {
    ast_type_t* type = ast_type_allocate();
    type->name = token_text(parser_cnext(parser, catch, 1, TK_NAMING));
    token_t* token = parser_next(parser);
    if (token->type == TK_LESS) {
        type->gen = parser_parse_type(parser, catch);
        parser_cnext(parser, catch, 1, TK_GREAT);
    } else parser_prev(parser);
    return type;
}

ast_variable_t* parser_parse_variable_define(parser_t* parser, jmp_buf catch) {
    ast_variable_t* variable = ast_variable_allocate();
    variable->name = token_text(parser_cnext(parser, catch, 1, TK_NAMING));
    parser_cnext(parser, catch, 1, TK_COLON);
    variable->type = parser_parse_type(parser, catch);
    return variable;
}

parser_parse_result_t parser_parse(token_t* token) {
    // Сбрасываем ошибки
    parser_error = NULL;
    // Инициализация
    parser_t* parser = malloc(sizeof(parser_t));
    parser->token = token;
    // Создаём контекст
    ast_context_t* context = ast_context_allocate();
    parser->context = context;
    // Установка обработчика ошибок
    jmp_buf catch;
    if (setjmp(catch) == 0) {
        // Проходимся по токенам
        while (parser->token->type != TK_EOF) {
            // Пропускаем бесполезные токены
            parser_skip(parser);
            // Парсим функции
            ast_function_t* function = parser_parse_function(parser, catch);
            if (function != NULL) {
                size_t size = sizeof(ast_function_t*) * ++context->func;
                void* tmp = malloc(size);
                memcpy(tmp, context->funs, size - sizeof(ast_function_t*));
                free(context->funs);
                context->funs = tmp;
                context->funs[context->func - 1] = function;
            }
        }
    }
    // Выход
    free(parser);
    parser_parse_result_t result = { context, parser_error };
    return result;
}