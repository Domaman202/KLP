#include <lexer.h>
#include <error.h>

#include <stdlib.h>
#include <string.h>

int lexer_line, lexer_symbol;
char* lexer_source;

void lexer_shift() {
    lexer_source++;
    lexer_symbol++;
}

void lexer_backshift() {
    lexer_source--;
    lexer_symbol--;
}

token_t* lexer_next(token_t* prev) {
    token_t* token = token_allocate(lexer_source, prev);
    char c = *lexer_source;
    //
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_')) {
        token->tsize = -1;
        do {
            parse: {
                c = *lexer_source;
                lexer_shift();
                token->tsize++;
                if (*lexer_source == ':') {
                    lexer_shift();
                    if (*lexer_source == ':') {
                        lexer_shift();
                        token->tsize += 2;
                        goto parse;
                    } else lexer_backshift();
                }
            }
        } while ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_') || (c >= '0' && c <= '9'));
        lexer_backshift();
        token->type = TK_NAMING;
    } else if (c >= '0' && c <= '9') {
        token->tsize = -1;
        do {
            c = *lexer_source;
            lexer_shift();
            token->tsize++;
        } while (c >= '0' && c <= '9');
        lexer_backshift();
        token->type = TK_NUMBER;
    } else {
        switch (c) {
            case '(':
                lexer_shift();
                token->type = TK_OPEN_BRACKET;
                break;
            case ')':
                lexer_shift();
                token->type = TK_CLOSE_BRACKET;
                break;
            case '[':
                lexer_shift();
                token->type = TK_OPEN_CUBE_BRACKET;
                break;
            case ']':
                lexer_shift();
                token->type = TK_CLOSE_CUBE_BRACKET;
                break;
            case '{':
                lexer_shift();
                token->type = TK_OPEN_FIGURAL_BRACKET;
                break;
            case '}':
                lexer_shift();
                token->type = TK_CLOSE_FIGURAL_BRACKET;
                break;
            case '.':
                lexer_shift();
                token->type = TK_DOT;
                break;
            case ',':
                lexer_shift();
                token->type = TK_COMMA;
                break;
            case '!':
                lexer_shift();
                if (*lexer_source == '=') {
                    lexer_shift();
                    token->tsize = 2;
                    token->type = VTK_NEQ;
                } else token->type = TK_EXCLAMINATION;
                break;
            case ':':
                lexer_shift();
                token->type = TK_COLON;
                break;
            case ';':
                lexer_shift();
                token->type = TK_SEMICOLON;
                break;
            case '/':
                lexer_shift();
                token->type = TK_SLASH;
                break;
            case '\\':
                lexer_shift();
                token->type = TK_SLASH;
                break;
            case '|':
                lexer_shift();
                if (*lexer_source == '|') {
                    lexer_shift();
                    token->tsize = 2;
                    token->type = VTK_OR;
                } else token->type = TK_PIPE;
                break;
            case '&':
                lexer_shift();
                if (*lexer_source == '&') {
                    lexer_shift();
                    token->tsize = 2;
                    token->type = VTK_AND;
                } else token->type = TK_AMPERSAND;
                break;
            case '^':
                lexer_shift();
                token->type = TK_CIRCUMFLEX;
                break;
            case '#':
                lexer_shift();
                token->type = TK_SHARP;
                break;
            case '@':
                lexer_shift();
                token->type = TK_DOG;
                break;
            case '=':
                lexer_shift();
                if (*lexer_source == '=') {
                    lexer_shift();
                    token->tsize = 2;
                    token->type = VTK_EQ;
                } else token->type = TK_ASSIGN;
                break;
            case '+':
                lexer_shift();
                token->type = TK_PLUS;
                break;
            case '-':
                lexer_shift();
                token->type = TK_MINUS;
                break;
            case '*':
                lexer_shift();
                token->type = TK_STAR;
                break;
            case '~':
                lexer_shift();
                token->type = TK_TILDE;
                break;
            case '>':
                lexer_shift();
                if (*lexer_source == '>') {
                    lexer_shift();
                    token->tsize = 2;
                    token->type = VTK_RIGHT_SHIFT;
                } else if (*lexer_source == '=') {
                    lexer_shift();
                    token->tsize = 2;
                    token->type = VTK_GOE;
                } else token->type = TK_GREAT;
                break;
            case '<':
                lexer_shift();
                if (*lexer_source == '<') {
                    lexer_shift();
                    token->tsize = 2;
                    token->type = VTK_LEFT_SHIFT;
                } else if (*lexer_source == '=') {
                    lexer_shift();
                    token->tsize = 2;
                    token->type = VTK_LOE;
                } else token->type = TK_LESS;
                break;
            case '\'':
                lexer_source += 2;
                token->tsize = 3;
                token->type = *lexer_source == '\'' ? TK_CHAR : TK_ERROR;
                lexer_shift();
                break;
            case '"':
                lexer_shift();
                do {
                    c = *lexer_source;
                    lexer_shift();
                    token->tsize++;
                    if (c == '\\' && *lexer_source == '"') {
                        lexer_shift();
                        token->tsize++;
                    }
                } while (c != '"');
                token->type = TK_STRING;
                break;
            case ' ':
            case '\t':
                token->tsize = -1;
                do {
                    c = *lexer_source;
                    lexer_shift();
                    token->tsize++;
                } while (c == ' ' || c == '\t');
                token->type = TK_SPACE;
                lexer_backshift();
                break;
            case '\n':
                lexer_source++;
                lexer_line++;
                lexer_symbol = 0;
                token->tsize = 0;
                token->type = TK_NEWLINE;
                break;
            case '\0':
                token->tsize = 0;
                token->next = NULL;
                token->type = TK_EOF;
                break;
            default:
                lexer_shift();
                token->type = TK_ERROR;
                throw_invalid_token(token); // todo: custom error
        }
    }
    //
    return token;
}

token_t* lexer_lex(char* src) {
    // Инициализация
    lexer_source = src;
    token_t* first = malloc(sizeof(token_t));
    token_t* last = first;
    // Проходимся по символам
    while (1) {
        last = lexer_next(last);
        if (last->type == TK_EOF) {
            break;
        }
    }
    // Выход
    return first->next;
}