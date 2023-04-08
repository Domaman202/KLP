#include <token.h>
#include <lexer.h>

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

token_t* token_allocate(char* str, token_t* prev) {
    token_t* token = malloc(sizeof(token_t));
    prev->next = token;
    token->prev = prev;
    token->next = NULL;
    token->text = str;
    token->tsize = 1;
    token->line = lexer_line;
    token->symbol = lexer_symbol;
    return token;
}

char* token_text(token_t* token) {
    uint16_t tsize = token->tsize;
    char* text = malloc(tsize + 1);
    memcpy(text, token->text, tsize);
    text[tsize] = '\0';
    return text;
}
