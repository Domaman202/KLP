#include <util.h>
#include <lexer.h>
#include <error.h>
#include <parser.h>
#include <cleaner.h>
#include <builder.h>

#include <string.h>
#include <stdarg.h>

token_t* parser_token;

bool parser_skip_space() {
    token_t* token = parser_token; 
    while (token->type == TK_SPACE)
        token = token->next;
    bool r = parser_token == token;
    parser_token = token;
    return r;
}

bool parser_skip_nl() {
    token_t* token = parser_token; 
    while (token->type == TK_NEWLINE)
        token = token->next;
    bool r = parser_token == token;
    parser_token = token;
    return r;
}

bool parser_skip() {
    token_t* token = parser_token; 
    while (token->type == TK_SPACE || token->type == TK_NEWLINE)
        token = token->next;
    bool r = parser_token == token;
    parser_token = token;
    return r;
}

void parser_prev() {
    if (parser_token->prev != NULL) {
        parser_token = parser_token->prev;
    }
}

token_t* parser_next() {
    parser_skip_space();
    token_t* token = parser_token;
    if (token->next != NULL)
        parser_token = token->next;
    return token;
}

token_t* parser_cnext__(char* __fun, char* __file, uint16_t __line, unsigned int argc, ...) {
    // Инициализация
    va_list types;
    va_start(types, argc);
    // Проходимся по списку требуемых аргументов
    token_t* token = parser_next();
    for (; argc; argc--) {
        token_type_t type = va_arg(types, token_type_t);
        if (token->type == type) {
            va_end(types);
            return token;
        }
    }
    // Кидаем ошибку
    throw_invalid_token_(__fun, __file, __line, token);
}

ast_context_t* parser_parse_context(bool inbody, bool instruct) {
    ast_context_t* context = ast_context_allocate();
    ast_body_t* ans = ast_body_allocate();
    // Парсим модуль
    context->module = parser_parse_module();
    // Проходимся по токенам
    while (parser_token->type != TK_EOF) {
        // Пропускаем бесполезные токены
        parser_skip();
        // Парсим аннотацию
        void* annotation = parser_tryparse_annotation();
        if (annotation) {
            ast_body_add(ans, annotation);
            continue;
        }
        // Пропускаем бесполезные токены
        parser_skip();
        // Парсим переменную
        void* variable = parser_tryparse_variable(ans, inbody, !inbody);
        if (variable) {
            context->vars = (void*) util_reallocadd((void*) context->vars, variable, ++context->varc);
            goto reset;
        }
        // Пропускаем бесполезные токены
        parser_skip();
        // Парсим функцию
        void* function = parser_tryparse_function(ans);
        if (function) {
            context->funs = (void*) util_reallocadd((void*) context->funs, function, ++context->func);
            goto reset;
        }
        // Пропускаем бесполезные токены
        parser_skip();
        // Парсим структуру
        if (!instruct) { // Невозможно создавать под-структуры (todo)
            void* structure = parser_tryparse_struct(ans);
            if (structure) {
                context->structs = (void*) util_reallocadd((void*) context->structs, structure, ++context->structc);
                goto reset;
            }
        }
        // Пропускаем бесполезные токены
        parser_skip();
        // Пропускаем бесполезные токены
        parser_skip();
        // Если это парсинг тела проверяем конец тела
        if (inbody && parser_token->type == TK_CLOSE_FIGURAL_BRACKET) {
            // Выходим
            break;
        }
        // Если ничего не смогли разобрать
        if (parser_token->type == TK_EOF)
            break; // Если это конец файла - выходим
        // Если это не конец файла - кидаем ошибку
        throw_invalid_token(parser_token);
        // Сбрасываем аннотации
        reset: {
            ans = ast_body_allocate();
        }
    }
    // Выход
    return context;
}

ast_ac_t* parser_tryparse_annotation() {
    token_t* token = parser_next();
    if (token->type == TK_DOG)
        return parser_parse_ac(true);
    parser_prev();
    return NULL;
}

char* parser_parse_module() {
    token_t* token = parser_next();
    if (token->type == TK_NAMING && util_token_cmpfree(token, "module")) {
        // Парсинг названия модуля
        return token_text(parser_cnext(1, TK_NAMING));
    } else throw_invalid_token(token);
    return NULL;
}

ast_struct_t* parser_tryparse_struct(ast_body_t* ans) {
    token_t* token = parser_next();
    if (token->type == TK_NAMING && util_token_cmpfree(token, "struct")) {
        // Парсинг структуры
        ast_struct_t* structure = ast_struct_allocate();
        // Сохраняем имя
        structure->name = token_text(parser_cnext(1, TK_NAMING));
        // Проверяем начало тела структуры
        token_t* token = parser_cnext(2, TK_COLON, TK_OPEN_FIGURAL_BRACKET);
        // Сохраняем предка если таковой имеется
        if (token->type == TK_COLON) {
            // Сохраняем имя предка
            structure->parent = token_text(parser_cnext(1, TK_NAMING));
            // Проверка начала тела структуры
            parser_cnext(1, TK_OPEN_FIGURAL_BRACKET);
        }
        // Парсим тело структуры
        ast_context_t* context = parser_parse_context(true, true);
        structure->varc = context->varc;
        structure->vars = context->vars;
        structure->func = context->func;
        structure->funs = context->funs;
        // Проверка на конец структуры
        parser_cnext(1, TK_CLOSE_FIGURAL_BRACKET);
        // Выход
        structure->expr.annotations = ans;
        return structure;
    } else parser_prev();
    return NULL;
}

ast_function_t* parser_tryparse_function(ast_body_t* ans) {
    token_t* token = parser_next();
    if (token->type == TK_NAMING && util_token_cmpfree(token, "fun")) {
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
                // Кидаем ошибку
                throw_invalid_token(token);
            }
        } else {
            parser_prev();
            // Проверка начала функции
            parser_cnext(1, TK_OPEN_FIGURAL_BRACKET);
            // Парсим тело функции
            ast_body_t* actions = parser_parse_body();
            // // Добавляем return при необходимости // todo: А оно нам тут надо?
            // ast_expr_t* expr = actions->exprs;
            // if (expr) { // Проверяем существует ли в функции хоть одно выражение
            //     // Находим последнее выражение функции
            //     while (expr->next) expr = expr->next;
            //     // Проверяем его тип
            //     if (expr->type != AST_RETURN) { // Если это выражение не возврат из функции
            //         // То добавляем возврат из функции
            //         if (strcmp(function->rettype->name, "void")) { // Проверяем что должна возвращать функция
            //             // Если что-то - добавляем выражение возврата и передаём в него в качестве аргумента последнее выражение
            //             *(expr->prev ? &expr->prev->next : &actions->exprs) = (void*) ast_return_allocate(expr);
            //         } else {
            //             // Если ничего - добавляем выражение возврата в конец
            //             ast_body_add(actions, (void*) ast_return_allocate(NULL));
            //         }
            //     }
            // } else {
            //     // Если в функции нет ни одного выражения добавляем return
            //     actions->exprs = (void*) ast_return_allocate((strcmp(function->rettype->name, "void")) ? expr : NULL);
            // }
            // Собираем выражения в функции
            function->body = ast_body_allocate();
            builder_build_body_cycle(function->body, actions);
        }
        // Выход
        function->expr.annotations = ans;
        return function;
    } else parser_prev();
    return NULL;
}

ast_variable_t* parser_tryparse_variable(ast_body_t* ans, bool inbody, bool global) {
    token_t* token = parser_next();
    if (token->type == TK_NAMING) {
        if (util_token_cmpfree(token, "var")) {
            return parser_parse_variable(ans, inbody, global);
        } else parser_prev();
    } else parser_prev();
    return NULL;
}

ast_variable_t* parser_parse_variable(ast_body_t* ans, bool inbody, bool global) {
    // Парсинг переменной
    ast_variable_t* variable;
    // Определние типа
    token_t* token = parser_cnext(1, TK_NAMING);
    bool type_defined = parser_cnext(2, TK_COLON, TK_ASSIGN)->type == TK_COLON;
    //
    if (type_defined) {
        parser_token = token;
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
        token_t* token_value = parser_next();
        if (token_value->type == TK_NAMING && util_token_cmpfree(token_value, "ext")) {
            if (!inbody && global) {
                variable->external = true;
            } else {
                // Кидаем ошибку (локальная переменная не может быть external)
                throw_invalid_token(token_value);
            }
        } else {
            if (inbody || !(global)) {
                parser_prev();
                variable->assign = parser_parse_expr(false);
            } else {
                // Кидаем ошибку (переменной нельзя присвоить значение)
                throw_invalid_token(token_assign);
            }
        }
    } else parser_prev();
    // Выход
    variable->expr.annotations = ans;
    return variable;
}

ast_body_t* parser_parse_body() {
    // Инициализация
    ast_body_t* body = ast_body_allocate();
    //
    while (1) {
        // Парсим выражение
        ast_expr_t* expr = parser_parse_expr(true);
        if (expr) { // Если выражение есть
            // Добавляем его
            ast_body_add(body, expr);
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
                // Кидаем ошибку
                throw_invalid_token(token);
        }
    }
}

ast_expr_t* parser_parse_expr(bool bodyparse) {
    // Инициализация
    ast_body_t* body = ast_body_allocate();
    // Перебираем все токены
    while (1) {
        token_t* token = parser_next();
        switch (token->type) {
            case TK_OPEN_BRACKET:
                parser_prev();
                ast_body_add(body, (void*) parser_parse_ac(false));
                break;
            case TK_OPEN_CUBE_BRACKET:
                // Парсинг указателя/массива
                ast_body_add(body, parser_parse_expr(false));
                // Добавляем операцию
                ast_math_t* dereference = ast_math_allocate(MOP_DEREFERENCE_GET);
                ast_body_add(body, (void*) dereference);
                // Парсинг сдвига/индекса
                token = parser_next();
                if (token->type == TK_COMMA) {
                    // Если есть запятая - есть индекс
                    ast_body_add(body, (void*) ast_value_allocate(AST_NAMING, (uintptr_t) token_text(parser_cnext(1, TK_NAMING))));
                    parser_cnext(1, TK_CLOSE_CUBE_BRACKET);
                } else if (token->type == TK_CLOSE_CUBE_BRACKET) {
                    // Если скобка закрывается - разыминовывание
                    ast_body_add(body, ast_empty_allocate());
                } else {
                    // Кидаем ошибку
                    throw_invalid_token(token);
                }
                // Проверяем присваивание
                token = parser_next();
                if (token->type == TK_ASSIGN) {
                    // Если это присваивание
                    dereference->operation = MOP_DEREFERENCE_SET;
                }
                parser_prev();
                // Выход
                break;
            case TK_OPEN_FIGURAL_BRACKET:
                ast_body_add(body, (ast_expr_t*) parser_parse_body());
                break;
            case TK_CLOSE_BRACKET:
            case TK_CLOSE_CUBE_BRACKET:
            case TK_CLOSE_FIGURAL_BRACKET:
            case TK_COMMA:
            case TK_SEMICOLON:
            case TK_NEWLINE:
                parser_prev();
                return cleaner_clean_expression((void*) body);
            case TK_NAMING: {
                if (bodyparse) { // Если это парсинг тела, то проверяем дальше
                    if (util_token_cmpfree(token, "var")) {
                        ast_body_add(body, (void*) parser_parse_variable(NULL, true, false));
                        break;
                    } else if (util_token_cmpfree(token, "if")) {
                        // Парсим if
                        ast_if_t* if_ = ast_if_allocate();
                        // Парсим условие
                        parser_cnext(1, TK_OPEN_FIGURAL_BRACKET);
                        if_->condition = (void*) parser_parse_body();
                        // Проверяем then
                        token = parser_cnext(1, TK_NAMING);
                        if (!util_token_cmpfree(token, "then")) {
                            throw_invalid_token(token);
                        }
                        // Парсим действие
                        parser_cnext(1, TK_OPEN_FIGURAL_BRACKET);
                        if_->action = (void*) parser_parse_body();
                        // Проверяем else
                        token = parser_next();
                        if (token->type == AST_NAMING && util_token_cmpfree(token, "else")) {
                            // Если есть else, парсим действие
                            parser_cnext(1, TK_OPEN_FIGURAL_BRACKET);
                            if_->else_action = (void*) parser_parse_body();
                        } else parser_prev();
                        // Добавляем if
                        ast_body_add(body, (void*) if_);
                        // Выход
                        break;
                    } else if (util_token_cmpfree(token, "while")) {
                        // Парсим while
                        ast_while_t* while_ = ast_while_allocate();
                        // Парсим условие
                        parser_cnext(1, TK_OPEN_FIGURAL_BRACKET);
                        while_->condition = (void*) parser_parse_body();
                        // Проверяем do
                        token = parser_cnext(1, TK_NAMING);
                        if (!util_token_cmpfree(token, "do")) {
                            throw_invalid_token(token);
                        }
                        // Парсим действие
                        parser_cnext(1, TK_OPEN_FIGURAL_BRACKET);
                        while_->action = (void*) parser_parse_body();
                        // Добавляем while
                        ast_body_add(body, (void*) while_);
                        // Выход
                        break;
                    } else if (util_token_cmpfree(token, "return")) {
                        // Парсим return 
                        ast_return_t* ret = ast_return_allocate();
                        // Парсим значение
                        ret->value = parser_parse_expr(false);
                        // Добавляем return
                        ast_body_add(body, (void*) ret);
                        // Выход
                        break;
                    } 
                }
                // Парсим значение
            }
            case TK_NUMBER:
            case TK_CHAR:
            case TK_STRING:
                ast_body_add(body, (void*) ast_value_allocate((ast_expr_type_t) token->type, (uintptr_t) token_text(token)));
                break;
            case TK_GREAT:
            case VTK_RIGHT_SHIFT:
            case TK_LESS:
            case VTK_LEFT_SHIFT:
            case VTK_AND:
            case VTK_OR:
            case TK_CIRCUMFLEX:
            case TK_PLUS:
            case TK_MINUS:
            case TK_STAR:
            case TK_EXCLAMINATION:
            case TK_ASSIGN:
            case VTK_EQ:
            case VTK_NEQ:
            case VTK_GOE:
            case VTK_LOE:
            case TK_AMPERSAND:
                ast_body_add(body, (void*) ast_math_allocate((ast_math_oper_t) token->type));
                break;
            case TK_DOG:
                ast_body_add(body, (void*) parser_parse_ac(true));
                break;
            default:
                throw_invalid_token(token);
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

ast_ac_t* parser_parse_ac(bool annotation) {
    // Парсим имя
    ast_ac_t* ac = ast_ac_allocate(annotation ? AST_ANNOTATION : AST_CALL, annotation ? token_text(parser_cnext(1, TK_NAMING)) : NULL);
    // Парсинг аргументов
    parser_cnext(1, TK_OPEN_BRACKET);
    while (1) {
        ast_body_add(ac->args, parser_parse_expr(false));
        // Парсинг аргументов
        token_t* token = parser_cnext(2, TK_COMMA, TK_CLOSE_BRACKET);
        if (token->type == TK_COMMA) { // если есть запятая - парсим следующий аргумент
            continue;
        } else if (token->type == TK_CLOSE_BRACKET) { // если закрывается скобка - конец выражения вызова функции
            break;
        }
    }
    // Выход
    return ac;
}

ast_variable_t* parser_parse_var_or_arg_define() {
    ast_variable_t* variable = ast_variable_allocate();
    variable->name = token_text(parser_cnext(1, TK_NAMING));
    parser_cnext(1, TK_COLON);
    variable->type = parser_parse_type();
    return variable;
}

ast_context_t* parser_parse(token_t* token) {
    // Инициализация
    parser_token = token;
    // Парсим контекст
    return parser_parse_context(false, false);
}