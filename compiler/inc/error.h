#ifndef __ERROR_H__
#define __ERROR_H__

#include <ast.h>

/* PUBLIC */

#define THROW_INFO (void*) __FUNCTION__, (void*) __FILE__, __LINE__

void catch_init();
#define throw_invalid_token(token) throw_invalid_token_(THROW_INFO, token)
#define throw_invalid_ast(expression) throw_invalid_ast_(THROW_INFO, expression)
#define throw_unknown_error() throw_unknown_error_(THROW_INFO)

/* THROWS */

void throw_invalid_token_(char* __fun, char* __file, uint16_t __line, token_t* token) __attribute__((__noreturn__));
void throw_invalid_ast_(char* __fun, char* __file, uint16_t __line, ast_expr_t* expression) __attribute__((__noreturn__));
void throw_unknown_error_(char* __fun, char* __file, uint16_t __line) __attribute__((__noreturn__));;

#endif