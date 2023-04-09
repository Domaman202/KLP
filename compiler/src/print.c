#include <print.h>

#include <stdlib.h>

FILE* print_stream;

void print_init() {
    print_stream = stdout;
    // print_stream = fopen("out.log", "w");
}

void token_print(token_t* token, bool nss) {
    if (nss || token->type != TK_SPACE) {
        char* text = token_text(token);
        fprintf(print_stream, "[\n\tType: 0x%x\n\tSize: %d\n\tText: %s\n]\n", token->type, token->tsize, text);
        free(text);
    }
}

void ast_print_indent(size_t indent) {
    while (indent-- > 0)
        fprintf(print_stream, "|\t");
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
                ast_ac_print(indent, (void*) expression);
                break;
            case AST_STRUCT:
                ast_struct_print(indent, (void*) expression);
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
            case AST_IF:
                ast_if_print(indent, (void*) expression);
                break;
            case AST_WHILE:
                ast_while_print(indent, (void*) expression);
                break;
            case AST_RETURN:
                ast_return_print(indent, (void*) expression);
                break;
            case AST_NUMBER:
            case AST_CHAR:
            case AST_STRING:
            case AST_NAMING:
            case AST_TMP:
                ast_value_print(indent, (void*) expression);
                break;
            default:
                ast_print_indent(indent);
                fprintf(print_stream, "[Unknown expression]: 0x%x\n", expression->type);
                break;
        }
    }
}

void ast_null_print(size_t indent) {
    ast_print_indent(indent);
    fprintf(print_stream, "[Null]\n");
}

void ast_empty_print(size_t indent) {
    ast_print_indent(indent);
    fprintf(print_stream, "[Empty]\n");
}

void ast_context_print(size_t indent, ast_context_t* context) {
    ast_print_indent(indent);
    fprintf(print_stream, "[Context]\n");
    if (context->module) {
        ast_print_indent(indent);
        fprintf(print_stream, "[Module] %s\n", context->module);
    }
    if (context->varc) {
        for (uint16_t i = context->varc; i > 0; i--) {
            ast_variable_print(indent + 1, context->vars[context->varc - i]);
            if (i > 1) {
                ast_print_indent(indent + 1);
                fprintf(print_stream, "\n");
            }
        }
        if (context->func) {
            ast_print_indent(indent + 1);
            fprintf(print_stream, "\n");
        }
    }
    if (context->func) {
        for (uint16_t i = context->func; i > 0; i--) {
            ast_function_print(indent + 1, context->funs[context->func - i]);
            if (i > 1) {
                ast_print_indent(indent + 1);
                fprintf(print_stream, "\n");
            }
        }
        if (context->structc) {
            ast_print_indent(indent + 1);
            fprintf(print_stream, "\n");
        }
    }
    for (uint16_t i = context->structc; i > 0; i--) {
        ast_struct_print(indent + 1, context->structs[context->structc - i]);
        if (i > 1) {
            ast_print_indent(indent + 1);
            fprintf(print_stream, "\n");
        }
    }
}

void ast_ac_print(size_t indent, ast_ac_t* ac) {
    ast_print_indent(indent);
    fprintf(print_stream, ac->expr.type == AST_ANNOTATION ? "[Annotation]\n" : "[Call]\n");
    if (ac->expr.type == AST_ANNOTATION) {
        ast_print_indent(indent);
        fprintf(print_stream, "|\t[name]\t\"%s\"\n", (char*) ac->argument);
    } else ast_expr_print(indent + 1, ac->argument);
    ast_body_print(indent + 1, ac->args, "arguments");
    ast_annotation_print(indent, (void*) ac);
}

void ast_annotation_print(size_t indent, ast_expr_t* expression) {
    if (expression->annotations && expression->annotations->exprs) {
        ast_body_print(indent + 1, expression->annotations, "annotations");
    }
}

void ast_struct_print(size_t indent, ast_struct_t* structure) {
    ast_print_indent(indent);
    fprintf(print_stream, "[Structure]\n");
    ast_print_indent(indent);
    fprintf(print_stream, "|\t[name]\t\"%s\"\n", structure->name);
    if (structure->parent) {
        ast_print_indent(indent);
        fprintf(print_stream, "|\t[parent]\t\"%s\"\n", structure->parent);
    }
    if (structure->varc) {
        for (uint16_t i = structure->varc; i > 0; i--) {
            ast_variable_print(indent + 1, structure->vars[structure->varc - i]);
            if (i > 1) {
                ast_print_indent(indent + 1);
                fprintf(print_stream, "\n");
            }
        }
        if (structure->func) {
            ast_print_indent(indent + 1);
            fprintf(print_stream, "\n");
        }
    }
    for (uint16_t i = structure->func; i > 0; i--) {
        ast_function_print(indent + 1, structure->funs[structure->func - i]);
        if (i > 1) {
            ast_print_indent(indent + 1);
            fprintf(print_stream, "\n");
        }
    }
    ast_annotation_print(indent, (void*) structure);
}

void ast_function_print(size_t indent, ast_function_t* function) {
    ast_print_indent(indent);
    fprintf(print_stream, "[Function]\n");
    ast_print_indent(indent);
    fprintf(print_stream, "|\t[name]\t\"%s\"\n", function->name);
    ast_print_indent(indent);
    fprintf(print_stream, "|\t[args]\t");
    ast_argument_t* argument = function->args;
    while (argument) {
        fprintf(print_stream, "%s (", argument->name);
        ast_type_print(argument->type);
        fprintf(print_stream, ")");
        if (argument->expr.next)
            fprintf(print_stream, ", ");
        argument = (void*) argument->expr.next;
    }
    fprintf(print_stream, "\n");
    ast_print_indent(indent);
    fprintf(print_stream, "|\t[ret]\t");
    ast_type_print(function->rettype);
    fprintf(print_stream, "\n");
    ast_print_indent(indent);
    fprintf(print_stream, "|\t[ext]\t%d\n", function->external);
    if (!function->external)
        ast_body_print(indent + 1, function->body, "body");
    ast_annotation_print(indent, (void*) function);
}

void ast_variable_print(size_t indent, ast_variable_t* variable) {
    ast_print_indent(indent);
    fprintf(print_stream, "[Variable]\n");
    ast_print_indent(indent);
    fprintf(print_stream, "|\t[name]\t\"%s\"\n", variable->name);
    ast_print_indent(indent);
    fprintf(print_stream, "|\t[type]\t(");
    if (variable->type)
        ast_type_print(variable->type);
    else fprintf(print_stream, "unkown");
    fprintf(print_stream, ")\n");
    ast_print_indent(indent);
    fprintf(print_stream, "|\t[glb]\t%d\n", variable->global);
    if (variable->global) {
        ast_print_indent(indent);
        fprintf(print_stream, "|\t[ext]\t%d\n", variable->external);
    } else if (variable->assign)
        ast_expr_print(indent, variable->assign);
    ast_annotation_print(indent, (void*) variable);
}

void ast_body_print(size_t indent, ast_body_t* body, char* text) {
    ast_print_indent(indent);
    fprintf(print_stream, "[%s]\n", text);
    for (ast_expr_t* expr = body->exprs; expr != NULL; expr = expr->next) {
        ast_expr_print(indent + 1, expr);
        if (expr->next) {
            ast_print_indent(indent + 1);
            fprintf(print_stream, "\n");
        }
    }
    ast_annotation_print(indent, (void*) body);
}

void ast_math_print(size_t indent, ast_math_t* math) {
    ast_print_indent(indent);
    fprintf(print_stream, "[Math]\n");
    ast_print_indent(indent + 1);
    fprintf(print_stream, "[Operation]\t0x%x\n", math->operation);
    ast_expr_print(indent + 1, math->left);
    ast_expr_print(indent + 1, math->right);
    ast_annotation_print(indent, (void*) math);
}

void ast_if_print(size_t indent, ast_if_t* if_) {
    ast_print_indent(indent);
    fprintf(print_stream, "[If]\n");
    ast_print_indent(indent + 1);
    fprintf(print_stream, "[Condition]\n");
    ast_expr_print(indent + 1, if_->condition);
    ast_print_indent(indent + 1);
    fprintf(print_stream, "[Action]\n");
    ast_expr_print(indent + 1, if_->action);
    ast_print_indent(indent + 1);
    fprintf(print_stream, "[Else]\n");
    ast_expr_print(indent + 1, if_->else_action);
}

void ast_while_print(size_t indent, ast_while_t* while_) {
    ast_print_indent(indent);
    fprintf(print_stream, "[While]\n");
    ast_print_indent(indent + 1);
    fprintf(print_stream, "[Condition]\n");
    ast_expr_print(indent + 1, while_->condition);
    ast_print_indent(indent + 1);
    fprintf(print_stream, "[Action]\n");
    ast_expr_print(indent + 1, while_->action);
}

void ast_return_print(size_t indent, ast_return_t* ret) {
    ast_print_indent(indent);
    fprintf(print_stream, "[Return]\n");
    if (ret->value) { 
        ast_expr_print(indent, ret->value);
    }
}

void ast_value_print(size_t indent, ast_value_t* value) {
    ast_print_indent(indent);
    switch (value->expr.type) {
        case AST_NUMBER:
            fprintf(print_stream, "[Number]");
            break;
        case AST_CHAR:
            fprintf(print_stream, "[Char]");
            break;
        case AST_STRING:
            fprintf(print_stream, "[String]");
            break;
        case AST_NAMING:
            fprintf(print_stream, "[Naming]");
            break;
        case AST_TMP:
            fprintf(print_stream, "[Tmp]\t%lu\n", value->value);
            return;
        default:
            fprintf(print_stream, "[Value]\n");
            ast_print_indent(indent + 1);
            fprintf(print_stream, "[Type]\t0x%x\n", value->expr.type);
            ast_print_indent(indent + 1);
            fprintf(print_stream, "[Text]\t%s\n", value->text);
            return;
    }
    fprintf(print_stream, "\t%s\n", value->text);
    ast_annotation_print(indent, (void*) value);
}

void ast_type_print(ast_type_t* type) {
    fprintf(print_stream, "%s", type->name);
    if (type->gen != NULL) {
        fprintf(print_stream, "<");
        ast_type_print(type->gen);
        fprintf(print_stream, ">");
    }
}