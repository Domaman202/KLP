#include <print.h>

#include <stdio.h>
#include <stdlib.h>

void token_print(token_t* token, bool nss) {
    if (nss || token->type != TK_SPACE) {
        char* text = token_text(token);
        printf("[\n\tType: 0x%x\n\tSize: %d\n\tText: %s\n]\n", token->type, token->tsize, text);
        free(text);
    }
}

void ast_print_indent(size_t indent) {
    while (indent-- > 0)
        printf("|\t");
}

void ast_expr_print(size_t indent, ast_expr_t* expression) {
    switch (expression->type) {
        case AST_CONTEXT:
            ast_context_print(indent, (ast_context_t*) expression);
            break;
        case AST_FUNCTION:
            ast_function_print(indent, (ast_function_t*) expression);
            break;
        case AST_TYPE:
            ast_type_print((ast_type_t*) expression);
            break;
    }
}

void ast_context_print(size_t indent, ast_context_t* context) {
    ast_print_indent(indent);
    printf("[Context]\n");
    for (uint16_t i = 0; i < context->func; i++) {
        ast_function_print(indent + 1, context->funs[i]);
    }
}

void ast_function_print(size_t indent, ast_function_t* function) {
    ast_print_indent(indent);
    printf("[Function]\n");
    ast_print_indent(indent);
    printf("|name: \"%s\"\n", function->name);
    ast_print_indent(indent);
    printf("|args: ");
    for (uint8_t i = function->argc; i > 0; i--) {
        ast_variable_t* arg = function->args[function->argc - i];
        printf("%s (", arg->name);
        ast_type_print(arg->type);
        printf(")");
        if (i > 1) {
            printf("; ");
        }
    }
    printf("\n");
    ast_print_indent(indent);
    printf("|ret: ");
    ast_type_print(function->rettype);
    printf("\n");
}

void ast_type_print(ast_type_t* type) {
    printf("%s", type->name);
    if (type->gen != NULL) {
        printf("<");
        ast_type_print(type->gen);
        printf(">");
    }
}