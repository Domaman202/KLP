#include <ast.h>
#include <lexer.h>
#include <print.h>
#include <parser.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int main() {
    lexer_lex_result_t lres = lexer_lex("fun add(a: i32, b: i32): i32 = {}");
    token_t* token = lres.tokens;
    /// Lexer Print
    // printf("Lexer Result:\n");
    // while (token) {
    //     token_print(token, true);
    //     token = token->next;
    // }
    /// Lexer Errors Print
    if (lres.error) {
        printf("Lexer Errors:\n");
        token_print(lres.error, true);
    }
    /// Parser Print
    parser_parse_result_t pres = parser_parse(token);
    ast_context_print(pres.context);
    /// Parser Errors Print
    if (pres.error) {
        printf("Parser Errors:\n");
        token_print(pres.error, true);
    }
}