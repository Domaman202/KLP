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
        ast_null_print(indent);
    } else {
        switch (expression->type) {
            case AST_EMPTY:
                ast_empty_print(indent);
                break;
            case AST_CONTEXT:
                ast_context_print(indent, (void*) expression);
                break;
            case AST_ANNOTATION:
            case AST_CALL:
                ast_con_print(indent, (void*) expression);
                break;
            case AST_FUNCTION:
                ast_function_print(indent, (void*) expression);
                break;
            case AST_VARIABLE:
                ast_variable_print(indent, (void*) expression);
                break;
            case AST_TYPE:
                ast_type_print((void*) expression);
                break;
            case AST_BODY:
                ast_body_print(indent, (void*) expression, "Body");
                break;
                break;
            case AST_MATH:
                ast_math_print(indent, (void*) expression);
                break;
            case AST_NUMBER:
            case AST_CHAR:
            case AST_STRING:
            case AST_NAMING:
                ast_value_print(indent, (void*) expression);
                break;
            default:
                ast_print_indent(indent);
                printf("[Unknown expression]: 0x%x\n", expression->type);
                break;
        }
    }
}

void ast_null_print(size_t indent) {
    ast_print_indent(indent);
    printf("[Null]\n");
}

void ast_empty_print(size_t indent) {
    ast_print_indent(indent);
    printf("[Empty]\n");
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

void ast_con_print(size_t indent, ast_con_t* con) {
    ast_print_indent(indent);
    printf(con->expr.type == AST_ANNOTATION ? "[Annotation]\n" : "[Call]\n");
    ast_print_indent(indent);
    printf("|\t[name]\t\"%s\"\n", con->name);
    ast_body_print(indent + 1, con->args, "arguments");
    ast_annotation_print(indent, (void*) con);
}

void ast_annotation_print(size_t indent, ast_expr_t* expression) {
    if (expression->annotations && expression->annotations->exprs) {
        ast_body_print(indent + 1, expression->annotations, "annotations");
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
    if (!function->external)
        ast_body_print(indent + 1, function->body, "body");
    ast_annotation_print(indent, (void*) function);
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
    } else if (variable->assign)
        ast_expr_print(indent, variable->assign);
    ast_annotation_print(indent, (void*) variable);
}

void ast_body_print(size_t indent, ast_body_t* body, char* text) {
    ast_print_indent(indent);
    printf("[%s]\n", text);
    for (ast_expr_t* expr = body->exprs; expr != NULL; expr = expr->next) {
        ast_expr_print(indent + 1, expr);
        if (expr->next) {
            ast_print_indent(indent + 1);
            printf("\n");
        }
    }
    ast_annotation_print(indent, (void*) body);
}

void ast_math_print(size_t indent, ast_math_t* math) {
    ast_print_indent(indent);
    printf("[Math]\n");
    ast_print_indent(indent + 1);
    printf("[Operation]\t0x%x\n", math->operation);
    ast_expr_print(indent + 1, math->left);
    ast_expr_print(indent + 1, math->right);
    ast_annotation_print(indent, (void*) math);
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
    ast_annotation_print(indent, (void*) value);
}

void ast_type_print(ast_type_t* type) {
    printf("%s", type->name);
    if (type->gen != NULL) {
        printf("<");
        ast_type_print(type->gen);
        printf(">");
    }
}