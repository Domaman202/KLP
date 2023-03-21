#include <ss.h>
#include <ast.h>
#include <lexer.h>
#include <print.h>
#include <parser.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int main() {
    /// Source Read
    char *src;
    {
        FILE *f = fopen("../examples/test.kpl", "rb");
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

        src = malloc(fsize + 1);
        fread(src, fsize, 1, f);
        // fclose(f);

        src[fsize] = '\0';
    }
    printf("Source Read:\n%s\n", src);
    /// Lexer
    lexer_lex_result_t lres = lexer_lex(src);
    /// Lexer Print
    // token_t* token = lres.tokens;
    // printf("\nLexer Result:\n");
    // while (token) {
    //     token_print(token, true);
    //     token = token->next;
    // }
    /// Lexer Errors Print
    if (lres.error) {
        printf("Lexer Errors:\n");
        token_print(lres.error, true);
        return 1;
    }
    /// Parser
    parser_parse_result_t pres = parser_parse(lres.tokens);
    /// Parser Print
    printf("\nParser Result:\n");
    ast_context_print(0, pres.context);
    /// Parser Errors Print
    if (pres.error) {
        printf("\nParser Errors:\n");
        token_print(pres.error, true);
        return 1;
    }
    // /// SS
    // ss_ss_context(pres.context);
    // /// SS Print
    // printf("\nSS Result:\n");
    // ast_context_print(0, pres.context);    
}