#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <stdint.h>

typedef enum token_type token_type_t;
typedef struct token token_t;

enum token_type {
    // Скобки
    TK_OPEN_BRACKET             = 0x0, /* ( */
    TK_CLOSE_BRACKET            = 0x1, /* ) */
    TK_OPEN_CUBE_BRACKET        = 0x2, /* [ */
    TK_CLOSE_CUBE_BRACKET       = 0x3, /* ] */
    TK_OPEN_FIGURAL_BRACKET     = 0x4, /* { */
    TK_CLOSE_FIGURAL_BRACKET    = 0x5, /* } */

    // Стрелки
    TK_GREAT                    = 0x6,  /* >  */
    VTK_RIGHT_SHIFT             = 0x20, /* >> */
    TK_LESS                     = 0x7,  /* <  */
    VTK_LEFT_SHIFT              = 0x21, /* << */

    // Битовые операции
    TK_AMPERSAND                = 0x8, /* & */
    TK_PIPE                     = 0x9, /* | */
    TK_CIRCUMFLEX               = 0xA, /* ^ */
    TK_TILDE                    = 0xB, /* ~ */

    // Математические операции
    TK_PLUS                     = 0xC, /* + */
    TK_MINUS                    = 0xD, /* - */
    TK_STAR                     = 0xE, /* * */
    TK_SLASH                    = 0xF, /* / */

    // Остальные символы
    TK_DOT                      = 0x10, /* . */
    TK_COMMA                    = 0x11, /* , */
    TK_EXCLAMINATION            = 0x12, /* ! */ 
    TK_COLON                    = 0x13, /* : */
    TK_SEMICOLON                = 0x14, /* ; */
    TK_BACK_SLASH               = 0x15, /* \ */
    TK_SHARP                    = 0x16, /* # */
    TK_DOG                      = 0x17, /* @ */
    TK_ASSIGN                   = 0x18, /* = */

    // Остальные виртуальные символы
    VTK_EQ                      = 0x22, /* == */
    VTK_NEQ                     = 0x23, /* != */
    VTK_GOE                     = 0x24, /* >= */
    VTK_LOE                     = 0x25, /* <= */
    VTK_AND                     = 0x26, /* && */
    VTK_OR                      = 0x27, /* || */

    // Числа/Символы/Строки/Названия
    TK_NUMBER                   = 0x19, /* 0123456789   */
    TK_CHAR                     = 0x1A, /* 'x'          */
    TK_STRING                   = 0x1B, /* "DomamaN202" */
    TK_NAMING                   = 0x1C, /* i */

    // Спец. символы
    TK_SPACE                    = 0x1D, // Пробелы и табы
    TK_NEWLINE                  = 0x1E, // Перенос строки
    TK_EOF                      = 0x1F, // Конец файла

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

token_t* token_allocate(char* str, token_t* prev);
char* token_text(token_t* token);

#endif /* __TOKEN_H__ */