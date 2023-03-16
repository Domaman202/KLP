#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdint.h>

typedef enum token_type token_type_t;
typedef struct token token_t;
typedef struct lexer_next_result lexer_next_result_t;
typedef struct lexer_lex_result lexer_lex_result_t;

enum token_type {
    // Скобки
    TK_OPEN_BRACKET             = 0x0, /* ( */
    TK_CLOSE_BRACKET            = 0x1, /* ) */
    TK_OPEN_CUBE_BRACKET        = 0x2, /* [ */
    TK_CLOSE_CUBE_BRACKET       = 0x3, /* ] */
    TK_OPEN_FIGURAL_BRACKET     = 0x4, /* { */
    TK_CLOSE_FIGURAL_BRACKET    = 0x5, /* } */

    // Стрелки
    TK_GREAT                    = 0x10, /* > */
    TK_LESS                     = 0x11, /* < */

    // Битовые операции
    TK_AMPERSAND                = 0x20, /* & */
    TK_PIPE                     = 0x21, /* | */
    TK_CIRCUMFLEX               = 0x22, /* ^ */
    TK_TILDE                    = 0x23, /* ~ */


    // Остальные символы
    TK_DOT                      = 0x30, /* . */
    TK_COMMA                    = 0x31, /* , */
    TK_EXCLAMINATION            = 0x32, /* ! */ 
    TK_COLON                    = 0x33, /* : */
    TK_SEMICOLON                = 0x34, /* ; */
    TK_SLASH                    = 0x35, /* / */
    TK_BACK_SLASH               = 0x36, /* \ */
    TK_SHARP                    = 0x37, /* # */
    TK_DOG                      = 0x38, /* @ */
    TK_ASSIGN                   = 0x39, /* = */
    TK_PLUS                     = 0x3A, /* + */
    TK_MINUS                    = 0x3B, /* - */
    TK_STAR                     = 0x3C, /* * */

    // Числа/Символы/Строки/Названия
    TK_NUMBER                   = 0x40, /* 0123456789   */
    TK_CHAR                     = 0x41, /* 'x'          */
    TK_STRING                   = 0x42, /* "DomamaN202" */
    TK_NAMING                   = 0x43, /* i */

    // Спец. символы
    TK_SPACE                    = 0x50, // Пробелы и табы
    TK_NEWLINE                  = 0x51, // Перенос строки
    TK_EOF                      = 0x52, // Конец файла

    // Ошибка
    TK_ERROR                    = 0xFF
};

/*
    prev    - прошлый токен
    next    - следующий токен
    type    - тип токена
    text    - текст токена
    tsize   - размер текста токена
    line    - строка токена
    symbol  - начальный символ в строке токена
*/
struct token {
    struct token* prev;
    struct token* next;
    enum token_type type;
    char* text;
    uint16_t tsize;
    uint16_t line;
    uint16_t symbol;
};

struct lexer_next_result {
    char* str;
    struct token* token;
};

/*
    tokens  - успешные токены
    error   - ошибочный токен
*/
struct lexer_lex_result {
    struct token* tokens;
    struct token* error;
};

token_t* token_allocate(char* str, token_t* prev);
char* token_text(token_t* token);
lexer_next_result_t lexer_next(char* str, token_t* prev);
lexer_lex_result_t lexer_lex(char* src);

#endif /* __LEXER_H__ */