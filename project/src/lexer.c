#include <lexer.h>
#include <stdlib.h>

token_t* token_allocate(char* str, token_t* prev) {
    token_t* token = malloc(sizeof(token_t));
    prev->next = token;
    token->prev = prev;
    token->next = NULL;
    token->text = str;
    token->tsize = 0;
    token->line = prev->line;
    token->symbol = prev->symbol + 1;
    return token;
}

lexer_next_result_t lexer_next(char* str, token_t* prev) {
    token_t* token = token_allocate(str, prev);
    char c = *str;
    //
    if (c >= '0' && c <= '9') {
        do {
            c = *str;
            str++;
            token->tsize++;
        } while (c >= '0' && c <= '9');
        token->tsize--;
        token->type = TK_NUMBER;
    } else {
        switch (c) {
            case ' ':
            case '\t':
                do {
                    c = *str;
                    str++;
                    token->tsize++;
                } while (c == ' ' || c == '\t');
                token->tsize--;
                token->type = TK_SPACE;
                break;
            case '\0':
                token->type = TK_EOF;
                break;
            default:
                str++;
                token->tsize = 1;
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