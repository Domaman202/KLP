#include <print.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

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