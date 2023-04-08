#ifndef __LEXER_H__
#define __LEXER_H__

#include <token.h>

extern int lexer_line, lexer_symbol;
extern char* lexer_source;

void lexer_shift();
void lexer_backshift();
token_t* lexer_next(token_t* prev);
token_t* lexer_lex(char* src);

#endif /* __LEXER_H__ */