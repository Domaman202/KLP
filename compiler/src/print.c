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
    if (expression == NULL) {
        ast_print_indent(indent);
        printf("[Null]\n");
    } else {
        switch (expression->type) {
            case AST_CONTEXT:
                ast_context_print(indent, (ast_context_t*) expression);
                break;
            case AST_FUNCTION:
                ast_function_print(indent, (ast_function_t*) expression);
                break;
            case AST_VARIABLE:
                ast_variable_print(indent, (ast_variable_t*) expression);
                break;
            case AST_TYPE:
                ast_type_print((ast_type_t*) expression);
                break;
            case AST_BODY:
                ast_body_print(indent, (ast_body_t*) expression);
                break;
            case AST_MATH:
                ast_math_print(indent, (ast_math_t*) expression);
                break;
            case AST_NUMBER:
            case AST_CHAR:
            case AST_STRING:
            case AST_NAMING:
                ast_value_print(indent, (ast_value_t*) expression);
                break;
            default:
                ast_print_indent(indent);
                printf("[Unknown expression]: 0x%x\n", expression->type);
                break;
        }
    }
}

void ast_context_print(size_t indent, ast_context_t* context) {
    ast_print_indent(indent);
    printf("[Context]\n");
    if (context->varc) {
        for (uint16_t i = context->varc; i > 0; i--) {
            ast_variable_print(indent + 1, context->vars[context->varc - i]);
            if (i > 1) {
                ast_print_indent(indent + 1);
                printf("\n");
            }
        }
        if (context->func) {
            ast_print_indent(indent + 1);
            printf("\n");
        }
    }
    for (uint16_t i = context->func; i > 0; i--) {
        ast_function_print(indent + 1, context->funs[context->func - i]);
        if (i > 1) {
            ast_print_indent(indent + 1);
            printf("\n");
        }
    }
}

void ast_function_print(size_t indent, ast_function_t* function) {
    ast_print_indent(indent);
    printf("[Function]\n");
    ast_print_indent(indent);
    printf("|\t[name]\t\"%s\"\n", function->name);
    ast_print_indent(indent);
    printf("|\t[args]\t");
    for (uint8_t i = function->argc; i > 0; i--) {
        ast_variable_t* arg = function->args[function->argc - i];
        printf("%s (", arg->name);
        ast_type_print(arg->type);
        printf(")");
        if (i > 1) {
            printf(", ");
        }
    }
    printf("\n");
    ast_print_indent(indent);
    printf("|\t[ret]\t");
    ast_type_print(function->rettype);
    printf("\n");
    ast_print_indent(indent);
    printf("|\t[ext]\t%d\n", function->external);
    if (!function->external) {
        ast_body_print(indent, function->body);
    }
}

void ast_variable_print(size_t indent, ast_variable_t* variable) {
    ast_print_indent(indent);
    printf("[Variable]\n");
    ast_print_indent(indent);
    printf("|\t[name]\t\"%s\"\n", variable->name);
    ast_print_indent(indent);
    printf("|\t[type]\t(");
    if (variable->type)
        ast_type_print(variable->type);
    else printf("unkown");
    printf(")\n");
    ast_print_indent(indent);
    printf("|\t[glb]\t%d\n", variable->global);
    if (variable->global) {
        ast_print_indent(indent);
        printf("|\t[ext]\t%d\n", variable->external);
    } else if (variable->assign) {
        ast_expr_print(indent, variable->assign);
    }
}

void ast_body_print(size_t indent, ast_body_t* body) {
    ast_print_indent(indent);
    printf("[Body]\n");
    for (uint16_t i = 0; i < body->exprc; i++) {
        ast_expr_print(indent + 1, body->exprs[i]);
    }
}

void ast_math_print(size_t indent, ast_math_t* math) {
    ast_print_indent(indent);
    printf("[Math]\n");
    ast_print_indent(indent + 1);
    printf("[Operation] 0x%x\n", math->operation);
    ast_expr_print(indent + 1, math->left);
    if (math->right) {
        ast_expr_print(indent + 1, math->right);
    }
}

void ast_value_print(size_t indent, ast_value_t* value) {
    ast_print_indent(indent);
    switch (value->expr.type) {
        case AST_NUMBER:
            printf("[Number]");
            break;
        case AST_CHAR:
            printf("[Char]");
            break;
        case AST_STRING:
            printf("[String]");
            break;
        case AST_NAMING:
            printf("[Naming]");
            break;
        default:
            printf("[Value]\n");
            ast_print_indent(indent + 1);
            printf("[Type]\t0x%x\n", value->expr.type);
            ast_print_indent(indent + 1);
            printf("[Text]\t%s\n", value->text);
            return;
    }
    printf("\t%s\n", value->text);
}

void ast_type_print(ast_type_t* type) {
    printf("%s", type->name);
    if (type->gen != NULL) {
        printf("<");
        ast_type_print(type->gen);
        printf(">");
    }
}