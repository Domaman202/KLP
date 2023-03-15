#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <lexer.h>

void token_print(token_t token, bool nss) {
    if (nss || token.type != TK_SPACE) {
        uint16_t tsize = token.tsize;
        char* text = malloc(tsize + 1);
        memcpy(text, token.text, tsize);
        text[tsize] = '\0';
        printf("[\n\tType: 0x%x\n\tSize: %d\n\tText: %s\n]\n", token.type, tsize, text);
        free(text);
    }
}

int main() {
    lexer_lex_result_t result = lexer_lex("\"123\\\"312\" \t123");
    token_t* tk = result.tokens;
    while (tk) {
        token_print(*tk, true);
        tk = tk->next;
    }
}