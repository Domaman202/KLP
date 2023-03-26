#include <ap.h>

void ap_process_context(ast_context_t* context) {
    for (uint8_t i = 0; i < context->func; i++) {
        ap_process_function(context, context->funs[i]);
    }
}

void ap_process_function(ast_context_t* context, ast_function_t* function) {
    // todo: function annotation processor
    //
    if (function->body) {
        ast_expr_t* expression = function->body->exprs;
        while (expression) {
            ap_process_expression(context, function, function->body, expression);
            expression = expression->next;
        }
    }
}

void ap_process_expression(ast_context_t* context, ast_function_t* function, ast_body_t* body, ast_expr_t* expression) {
    if (expression->annotations) {
        ast_expr_t* annotation = expression->annotations->exprs;
        while (annotation) {
            ap_process(context, function, body, expression, (void*) annotation);
            annotation = annotation->next;
        }
    }
}

void ap_process(ast_context_t* context, ast_function_t* function, ast_body_t* body, ast_expr_t* expression, ast_annotation_t* annotation) {
    //todo:
}