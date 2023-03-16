#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <ast.h>
#include <lexer.h>
#include <parser.h>

void token_print(token_t* token, bool nss) {
    if (nss || token->type != TK_SPACE) {
        char* text = token_text(token);
        printf("[\n\tType: 0x%x\n\tSize: %d\n\tText: %s\n]\n", token->type, token->tsize, text);
        free(text);
    }
}

void ast_type_print(ast_type_t* type) {
    printf("%s", type->name);
    if (type->gen != NULL) {
        printf("<");
        ast_type_print(type->gen);
        printf(">");
    }
}

void ast_function_print(ast_function_t* function) {
    printf("fun %s(", function->name);
    for (uint8_t i = function->argc; i > 0; i--) {
        ast_variable_t* arg = function->args[function->argc - i];
        printf("%s: ", arg->name);
        ast_type_print(arg->type);
        if (i > 1) {
            printf(", ");
        }
    }
    printf("): ");
    ast_type_print(function->rettype);
    printf("\n");
}

void ast_context_print(ast_context_t* context) {
    for (uint16_t i = 0; i < context->func; i++) {
        ast_function_print(context->funs[i]);
    }
}

int main() {
    lexer_lex_result_t lres = lexer_lex("fun main(argc: ui32, args: ptr): i32");
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