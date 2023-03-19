#ifndef __AST_H__
#define __AST_H__

#include <stdint.h>
#include <stdbool.h>

typedef enum ast_expr_type ast_expr_type_t;
typedef struct ast_expr ast_expr_t;
typedef struct ast_context ast_context_t;
typedef struct ast_function ast_function_t;
typedef struct ast_variable ast_variable_t;
typedef struct ast_body ast_body_t;
typedef enum ast_math_oper ast_math_oper_t;
typedef struct ast_math ast_math_t;
typedef struct ast_naming ast_naming_t;
typedef struct ast_type ast_type_t;

enum ast_expr_type {
    AST_CONTEXT     = 0x0,
    AST_FUNCTION    = 0x1,
    AST_VARIABLE    = 0x2,
    AST_TYPE        = 0x3,
    AST_BODY        = 0x4,
    AST_MATH        = 0x5,
    AST_NAMING      = 0x6
};


struct ast_expr {
    ast_expr_type_t type;
};

struct ast_context {
    ast_expr_t expr;
    //
    uint8_t varc;
    ast_variable_t** vars;
    //
    uint8_t func;
    ast_function_t** funs;
};

struct ast_function {
    ast_expr_t expr;
    //
    char* name;
    uint8_t argc;
    ast_variable_t** args;
    ast_type_t* rettype;
    //
    bool external;
    //
    ast_body_t* body;
};

struct ast_variable {
    ast_expr_t expr;
    //
    char* name;
    ast_type_t* type;
    //
    bool external;
    bool global;
    //
    ast_expr_t* assign;
};

//

struct ast_body {
    ast_expr_t expr;
    //
    uint16_t exprc;
    ast_expr_t** exprs;
};

//

enum ast_math_oper {
    // Математические операции
    MOP_ADD         = 0xC,
    MOP_SUB         = 0xD,
    MOP_MUL         = 0xE,
    MOP_DIV         = 0xF,
    // Остальные операции
    MOP_ASSIGN      = 0x18,
};

struct ast_math {
    ast_expr_t expr;
    //
    ast_math_oper_t operation;
    //
    ast_expr_t* left;
    ast_expr_t* right;
};

struct ast_naming {
    ast_expr_t expr;
    //
    char* name;
};

//

struct ast_type {
    ast_expr_t expr;
    //
    char* name;
    ast_type_t* gen;
};

ast_context_t* ast_context_allocate();
ast_function_t* ast_function_allocate();
ast_variable_t* ast_variable_allocate();
ast_type_t* ast_type_allocate();
ast_body_t* ast_body_allocate();
ast_math_t* ast_math_allocate();
ast_naming_t* ast_naming_allocate();

void ast_body_add(ast_body_t* body, ast_expr_t* expression);

#endif /* __AST_H__ */