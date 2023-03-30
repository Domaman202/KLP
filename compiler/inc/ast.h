#ifndef __AST_H__
#define __AST_H__

#include <token.h>

#include <stdint.h>
#include <stdbool.h>

typedef enum ast_expr_type ast_expr_type_t;
typedef struct ast_expr ast_expr_t;
typedef struct ast_context ast_context_t;
typedef struct ast_ac ast_ac_t;
typedef struct ast_annotation ast_annotation_t;
typedef struct ast_namespace ast_namespace_t;
typedef struct ast_struct ast_struct_t;
typedef struct ast_function ast_function_t;
typedef struct ast_variable ast_variable_t;
typedef struct ast_body ast_body_t;
typedef enum ast_math_oper ast_math_oper_t;
typedef struct ast_math ast_math_t;
typedef struct ast_if ast_if_t;
typedef struct ast_while ast_while_t;
typedef struct ast_call ast_call_t;
typedef struct ast_return ast_return_t;
typedef struct ast_pointer ast_pointer_t;
typedef struct ast_value ast_value_t;
typedef struct ast_type ast_type_t;

enum ast_expr_type {
    AST_EMPTY       = 0x0,
    AST_CONTEXT     = 0x1,

    AST_ANNOTATION  = 0x2,
    AST_TYPE        = 0x3,

    AST_NAMESPACE   = 0x4,
    AST_STRUCT      = 0x5,
    AST_FUNCTION    = 0x6,
    AST_VARIABLE    = 0x7,

    AST_BODY        = 0x8,
    AST_MATH        = 0x9,
    AST_IF          = 0xA,
    AST_WHILE       = 0xB,
    AST_CALL        = 0xC,
    AST_RETURN      = 0xD,
    
    AST_NUMBER      = TK_NUMBER,    // 0x19
    AST_CHAR        = TK_CHAR,      // 0x1A
    AST_STRING      = TK_STRING,    // 0x1B
    AST_NAMING      = TK_NAMING,    // 0x1C
    
    AST_TMP         = 0x1D,
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
    //
    uint8_t nsc;
    ast_namespace_t** nss;
};

// Annotation/Call
struct ast_ac {
    ast_expr_t expr;
    //
    void* argument;
    ast_body_t* args;
};

struct ast_annotation {
    ast_expr_t expr;
    //
    char* name;
    ast_body_t* args;
};

struct ast_namespace {
    ast_context_t ctx;
    //
    char* name;
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
    MOP_NOT         = TK_EXCLAMINATION, // 0x12
    MOP_AND         = TK_AMPERSAND,     // 0x8
    MOP_OR          = TK_PIPE,          // 0x9
    MOP_XOR         = TK_CIRCUMFLEX,    // 0xA
    // Сдвиг
    MOP_RIGHT_SHIFT = VTK_RIGHT_SHIFT,  // 0x20
    MOP_LEFT_SHIFT  = VTK_LEFT_SHIFT,   // 0x21
    // Математические операции
    MOP_ADD         = TK_PLUS,  // 0xC
    MOP_SUB         = TK_MINUS, // 0xD
    MOP_MUL         = TK_STAR,  // 0xE
    MOP_DIV         = TK_SLASH, // 0xF
    // Сравнение
    MOP_EQ          = VTK_EQ,   // 0x22
    MOP_NEQ         = VTK_NEQ,  // 0x23
    MOP_GREAT       = TK_GREAT, // 0x6
    MOP_GOE         = VTK_GOE,  // 0x24
    MOP_LESS        = TK_LESS,  // 0x7
    MOP_LOE         = VTK_LOE,  // 0x25
    // Присваивание
    MOP_ASSIGN      = TK_ASSIGN,// 0x18
    // Ссылка
    MOP_REFERENCE   = TK_SHARP, // 0x16
    // Разыминовывание
    MOP_DEREFERENCE_SET = TK_OPEN_CUBE_BRACKET,     // 0x2
    MOP_DEREFERENCE_GET = TK_CLOSE_CUBE_BRACKET,    // 0x3
};

struct ast_math {
    ast_expr_t expr;
    //
    ast_math_oper_t operation;
    //
    ast_expr_t* left;
    ast_expr_t* right;
};

struct ast_if {
    ast_expr_t expr;
    //
    ast_expr_t* condition;
    ast_expr_t* action;
    ast_expr_t* else_action;
};

struct ast_while {
    ast_expr_t expr;
    //
    ast_expr_t* condition;
    ast_expr_t* action;
};

struct ast_call {
    ast_expr_t expr;
    //
    ast_expr_t* address;
    ast_body_t* args;
};

struct ast_return {
    ast_expr_t expr;
    //
    ast_expr_t* value;
};

struct ast_value {
    ast_expr_t expr;
    //
    union {
        char* text;
        uintptr_t value;
    };
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
ast_ac_t* ast_ac_allocate(ast_expr_type_t type, void* argument);
ast_namespace_t* ast_namespace_allocate(char* name);
ast_struct_t* ast_struct_allocate();
ast_function_t* ast_function_allocate();
ast_variable_t* ast_variable_allocate();
ast_type_t* ast_type_allocate();
ast_body_t* ast_body_allocate();
ast_math_t* ast_math_allocate(ast_math_oper_t operation);
ast_if_t* ast_if_allocate();
ast_while_t* ast_while_allocate();
ast_return_t* ast_return_allocate();
ast_value_t* ast_value_allocate(ast_expr_type_t type, uintptr_t text);

void ast_cute(ast_expr_t* prev, ast_expr_t* next, ast_body_t* body);
void ast_insert(ast_expr_t* expr, ast_expr_t* prev, ast_expr_t* next, ast_body_t* body);

void ast_set_next(ast_expr_t* expr, ast_expr_t* next);
void ast_set_prev(ast_expr_t* expr, ast_expr_t* prev);

void ast_add_annotation(ast_expr_t* expr, ast_ac_t* annotation);

void ast_body_add(ast_body_t* body, ast_expr_t* expr);
void ast_body_addback(ast_body_t* body, ast_expr_t* expr);

#endif /* __AST_H__ */