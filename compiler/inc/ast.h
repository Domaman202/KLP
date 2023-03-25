#ifndef __AST_H__
#define __AST_H__

#include <token.h>

#include <stdint.h>
#include <stdbool.h>

typedef enum ast_expr_type ast_expr_type_t;
typedef struct ast_expr ast_expr_t;
typedef struct ast_context ast_context_t;
typedef struct ast_ac ast_ac_t;
typedef struct ast_struct ast_struct_t;
typedef struct ast_function ast_function_t;
typedef struct ast_variable ast_variable_t;
typedef struct ast_body ast_body_t;
typedef enum ast_math_oper ast_math_oper_t;
typedef struct ast_math ast_math_t;
typedef struct ast_pointer ast_pointer_t;
typedef struct ast_value ast_value_t;
typedef struct ast_type ast_type_t;

enum ast_expr_type {
    AST_EMPTY       = 0x0,
    AST_CONTEXT     = 0x1,

    AST_ANNOTATION  = 0x2,
    AST_TYPE        = 0x3,

    AST_STRUCT      = 0x4,
    AST_FUNCTION    = 0x5,
    AST_VARIABLE    = 0x6,

    AST_BODY        = 0x7,
    AST_MATH        = 0x8,
    AST_CALL        = 0x9,
    
    AST_NUMBER      = TK_NUMBER,
    AST_CHAR        = TK_CHAR,
    AST_STRING      = TK_STRING,
    AST_NAMING      = TK_NAMING
};


struct ast_expr {
    ast_expr_t* prev;
    ast_expr_t* next;
    ast_expr_type_t type;
    ast_body_t* annotations;
};

struct ast_context {
    ast_expr_t expr;
    //
    uint8_t varc;
    ast_variable_t** vars;
    //
    uint8_t func;
    ast_function_t** funs;
    //
    uint8_t structc;
    ast_struct_t** structs;
};

// Annotation/Call
struct ast_ac {
    ast_expr_t expr;
    //
    char* name;
    ast_body_t* args;
};

struct ast_struct {
    ast_expr_t expr;
    //
    char* name;
    char* parent;
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
    ast_expr_t* exprs;
};

//

enum ast_math_oper {
    // Битовые операции
    MOP_NOT         = TK_EXCLAMINATION,
    MOP_AND         = TK_AMPERSAND,
    MOP_OR          = TK_PIPE,
    MOP_XOR         = TK_CIRCUMFLEX,
    MOP_RIGHT_SHIFT = VTK_RIGHT_SHIFT,
    MOP_LEFT_SHIFT  = VTK_LEFT_SHIFT,
    // Математические операции
    MOP_ADD         = TK_PLUS,
    MOP_SUB         = TK_MINUS,
    MOP_MUL         = TK_STAR,
    MOP_DIV         = TK_SLASH,
    // Сравнение
    MOP_EQ          = VTK_EQ,
    MOP_NEQ         = VTK_NEQ,
    MOP_GREAT       = TK_GREAT,
    MOP_GOE         = VTK_GOE,
    MOP_LESS        = TK_LESS,
    MOP_LOE         = VTK_LOE,
    // Разыминовывание
    MOP_DEREFERENCE = TK_OPEN_CUBE_BRACKET,
    // Присваивание
    MOP_ASSIGN      = TK_ASSIGN,
};

struct ast_math {
    ast_expr_t expr;
    //
    ast_math_oper_t operation;
    //
    ast_expr_t* left;
    ast_expr_t* right;
};

struct ast_value {
    ast_expr_t expr;
    //
    char* text;
};

//

struct ast_type {
    ast_expr_t expr;
    //
    char* name;
    ast_type_t* gen;
};

ast_expr_t* ast_empty_allocate();
ast_context_t* ast_context_allocate();
ast_ac_t* ast_ac_allocate(ast_expr_type_t type, char* name);
ast_struct_t* ast_struct_allocate();
ast_function_t* ast_function_allocate();
ast_variable_t* ast_variable_allocate();
ast_type_t* ast_type_allocate();
ast_body_t* ast_body_allocate();
ast_math_t* ast_math_allocate(ast_math_oper_t operation);
ast_value_t* ast_value_allocate(ast_expr_type_t type, char* text);

void ast_set_next(ast_expr_t* expr, ast_expr_t* next);
void ast_set_prev(ast_expr_t* expr, ast_expr_t* prev);
void ast_add_annotation(ast_expr_t* expr, ast_ac_t* annotation);

void ast_body_add(ast_body_t* body, ast_expr_t* expr);

#endif /* __AST_H__ */