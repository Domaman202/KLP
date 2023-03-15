#include <lexer.h>
#include <stdlib.h>

token_t* token_allocate(char* str, token_t* prev) {
    token_t* token = malloc(sizeof(token_t));
    prev->next = token;
    token->prev = prev;
    token->next = NULL;
    token->text = str;
    token->tsize = 1;
    token->line = prev->line;
    token->symbol = prev->symbol + 1;
    return token;
}

lexer_next_result_t lexer_next(char* str, token_t* prev) {
    token_t* token = token_allocate(str, prev);
    char c = *str;
    //
    if (c >= '0' && c <= '9') {
        token->tsize = -1;
        do {
            c = *str;
            str++;
            token->tsize++;
        } while (c >= '0' && c <= '9');
        str--;
        token->type = TK_NUMBER;
    } else {
        switch (c) {
            case '(':
                str++;
                token->type = TK_OPEN_BRACKET;
                break;
            case ')':
                str++;
                token->type = TK_CLOSE_BRACKET;
                break;
            case '[':
                str++;
                token->type = TK_OPEN_CUBE_BRACKET;
                break;
            case ']':
                str++;
                token->type = TK_CLOSE_CUBE_BRACKET;
                break;
            case '{':
                str++;
                token->type = TK_OPEN_FIGURAL_BRACKET;
                break;
            case '}':
                str++;
                token->type = TK_CLOSE_FIGURAL_BRACKET;
                break;
            case '.':
                str++;
                token->type = TK_DOT;
                break;
            case ',':
                str++;
                token->type = TK_COMMA;
                break;
            case '!':
                str++;
                token->type = TK_EXCLAMINATION;
                break;
            case ':':
                str++;
                token->type = TK_COLON;
                break;
            case ';':
                str++;
                token->type = TK_SEMICOLON;
                break;
            case '\'':
                str += 2;
                token->tsize = 3;
                token->type = *str == '\'' ? TK_CHAR : TK_ERROR;
                str++;
                break;
            case '"':
                str++;
                do {
                    c = *str;
                    str++;
                    token->tsize++;
                    if (c == '\\' && *str == '"') {
                        str++;
                        token->tsize++;
                    }
                } while (c != '"');
                token->type = TK_STRING;
                break;
            case ' ':
            case '\t':
                token->tsize = -1;
                do {
                    c = *str;
                    str++;
                    token->tsize++;
                } while (c == ' ' || c == '\t');
                token->type = TK_SPACE;
                str--;
                break;
            case '\0':
                token->tsize = 0;
                token->type = TK_EOF;
                break;
            default:
                str++;
                token->type = TK_ERROR;
                break;
        }
    }
    //
    lexer_next_result_t result = { str, token };
    return result;
}

lexer_lex_result_t lexer_lex(char* src) {
    // Текущий символ
    char* str = src;
    // Стартовый токен
    token_t* first = malloc(sizeof(token_t));
    // Текущий токен
    token_t* last = first;
    while (1) {
        lexer_next_result_t result = lexer_next(str, last);
        str = result.str;
        last = result.token;
        if (last->type == TK_EOF) {
            break;
        }
    }
    // Успешное выполнение
    lexer_lex_result_t result = { first->next, NULL };
    return result;
}