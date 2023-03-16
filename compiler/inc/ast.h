#ifndef __AST_H__
#define __AST_H__

#include <stdint.h>

typedef struct ast_type ast_type_t;
typedef enum ast_expr_type ast_expr_type_t;
typedef struct ast_expr ast_expr_t;
typedef struct ast_context ast_context_t;
typedef struct ast_function ast_function_t;
typedef struct ast_body ast_body_t;
typedef struct ast_variable ast_variable_t;

enum ast_expr_type {
    AST_CONTEXT,
    AST_FUNCTION,
    AST_BODY,
    AST_VARIABLE,
    AST_TYPE
};


struct ast_expr {
    ast_expr_type_t type;
};

struct ast_context {
    ast_expr_t expr;
    uint8_t func;
    ast_function_t** funs;
};

struct ast_function {
    ast_expr_t expr;
    char* name;
    uint8_t argc;
    ast_variable_t** args;
    ast_type_t* rettype;
    ast_body_t* body;
};

struct ast_body {
    ast_expr_t expr;
    uint16_t exprc;
    ast_expr_t** exprs;
};

struct ast_variable {
    ast_expr_t expr;
    char* name;
    ast_type_t* type;
};

struct ast_type {
    ast_expr_t expr;
    char* name;
    ast_type_t* gen;
};

ast_context_t* ast_context_allocate();
ast_function_t* ast_function_allocate();
ast_body_t* ast_body_allocate();
ast_variable_t* ast_variable_allocate();
ast_type_t* ast_type_allocate();

#endif /* __AST_H__ */