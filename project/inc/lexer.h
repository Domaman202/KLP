#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdint.h>

typedef enum token_type {
    // Скобки
    TK_OPEN_BRACKET             = 0x0, // (
    TK_CLOSE_BRACKET            = 0x1, // )
    TK_OPEN_CUBE_BRACKET        = 0x2, // [
    TK_CLOSE_CUBE_BRACKET       = 0x3, // ]
    TK_OPEN_FIGURAL_BRACKET     = 0x4, // {
    TK_CLSOE_FIGURAL_BRACKET    = 0x5, // }

    // Числа/Символы/Строки
    TK_NUMBER                   = 0x10, // 0123456789
    TK_CHAR                     = 0x11, // 'x'
    TK_STRING                   = 0x12, // "DomamaN202"

    // Остальные символы
    TK_DOT                      = 0x20, // .
    TK_COMMA                    = 0x21, // ,
    TK_EXCLAMINATION            = 0x22, // !
    TK_COLON                    = 0x23, // :
    TK_SEMICOLON                = 0x24, // ;

    // Спец. символы
    TK_SPACE                    = 0x30, // Пробелы и табы
    TK_EOF                      = 0x31, // Конец файла

    // Ошибка
    TK_ERROR                    = 0xFF
} token_type_t;

/*
    prev    - прошлый токен
    next    - следующий токен
    type    - тип токена
    text    - текст токена
    tsize   - размер текста токена
    line    - строка токена
    symbol  - начальный символ в строке токена
*/
typedef struct token {
    struct token* prev;
    struct token* next;
    enum token_type type;
    char* text;
    uint16_t tsize;
    uint16_t line;
    uint16_t symbol;
} token_t;

typedef struct lexer_next_result {
    char* str;
    struct token* token;
} lexer_next_result_t;

/*
    tokens  - успешные токены
    error   - ошибочный токен
*/
typedef struct lexer_lex_result {
    struct token* tokens;
    struct token* error;
} lexer_lex_result_t;

token_t* token_allocate(char* str, token_t* prev);
lexer_next_result_t lexer_next(char* str, token_t* prev);
lexer_lex_result_t lexer_lex(char* src);

#endif /* __LEXER_H__ */