#ifndef __LEXER_H__
#define __LEXER_H__

#include <token.h>

typedef struct lexer_next_result {
    char* str;
    struct token* token;
} lexer_next_result_t;

token_t* token_allocate(char* str, token_t* prev);
char* token_text(token_t* token);
lexer_next_result_t lexer_next(char* str, token_t* prev);
token_t* lexer_lex(char* src);

#endif /* __LEXER_H__ */