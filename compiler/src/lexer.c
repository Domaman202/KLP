#include <lexer.h>
#include <error.h>

#include <stdlib.h>
#include <string.h>

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

char* token_text(token_t* token) {
    uint16_t tsize = token->tsize;
    char* text = malloc(tsize + 1);
    memcpy(text, token->text, tsize);
    text[tsize] = '\0';
    return text;
}

lexer_next_result_t lexer_next(char* str, token_t* prev) {
    token_t* token = token_allocate(str, prev);
    char c = *str;
    //
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_')) {
        token->tsize = -1;
        do {
            c = *str;
            str++;
            token->tsize++;
        } while ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_') || (c >= '0' && c <= '9'));
        str--;
        token->type = TK_NAMING;
    } else if (c >= '0' && c <= '9') {
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
                if (*str == '=') {
                    str++;
                    token->tsize = 2;
                    token->type = VTK_NEQ;
                } else token->type = TK_EXCLAMINATION;
                break;
            case ':':
                str++;
                token->type = TK_COLON;
                break;
            case ';':
                str++;
                token->type = TK_SEMICOLON;
                break;
            case '/':
                str++;
                token->type = TK_SLASH;
                break;
            case '\\':
                str++;
                token->type = TK_SLASH;
                break;
            case '|':
                str++;
                token->type = TK_PIPE;
                break;
            case '&':
                str++;
                token->type = TK_AMPERSAND;
                break;
            case '^':
                str++;
                token->type = TK_CIRCUMFLEX;
                break;
            case '#':
                str++;
                token->type = TK_SHARP;
                break;
            case '@':
                str++;
                token->type = TK_DOG;
                break;
            case '=':
                str++;
                if (*str == '=') {
                    str++;
                    token->tsize = 2;
                    token->type = VTK_EQ;
                } else token->type = TK_ASSIGN;
                break;
            case '+':
                str++;
                token->type = TK_PLUS;
                break;
            case '-':
                str++;
                token->type = TK_MINUS;
                break;
            case '*':
                str++;
                token->type = TK_STAR;
                break;
            case '~':
                str++;
                token->type = TK_TILDE;
                break;
            case '>':
                str++;
                if (*str == '>') {
                    str++;
                    token->tsize = 2;
                    token->type = VTK_RIGHT_SHIFT;
                } else if (*str == '=') {
                    str++;
                    token->tsize = 2;
                    token->type = VTK_GOE;
                } else token->type = TK_GREAT;
                break;
            case '<':
                str++;
                if (*str == '<') {
                    str++;
                    token->tsize = 2;
                    token->type = VTK_LEFT_SHIFT;
                } else if (*str == '=') {
                    str++;
                    token->tsize = 2;
                    token->type = VTK_LOE;
                } else token->type = TK_LESS;
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
            case '\n':
                str++;
                token->tsize = 0;
                token->type = TK_NEWLINE;
                break;
            case '\0':
                token->tsize = 0;
                token->next = NULL;
                token->type = TK_EOF;
                break;
            default:
                str++;
                token->type = TK_ERROR;
                throw_invalid_token(token); // todo: custom error
        }
    }
    //
    lexer_next_result_t result = { str, token };
    return result;
}

token_t* lexer_lex(char* src) {
    // Инициализация
    char* str = src;
    token_t* first = malloc(sizeof(token_t));
    token_t* last = first;
    // Проходимся по символам
    while (1) {
        lexer_next_result_t result = lexer_next(str, last);
        str = result.str;
        last = result.token;
        if (last->type == TK_EOF)
            break;
    }
    // Выход
    return first->next;
}