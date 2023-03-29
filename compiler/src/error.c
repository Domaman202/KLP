#include <error.h>
#include <print.h>

#include <stdio.h>
#include <stdlib.h>

jmp_buf __error_catch;

void catch_init() {
    int value = setjmp(__error_catch);
    if (value) {
        exit(value);
    }
}

void throw_invalid_token_(char* __fun, char* __file, uint16_t __line, token_t* token) {
    fprintf(stderr, "[%s, %s, %d]\n", __fun, __file, __line);
    fprintf(stderr, "Invalid Token (type - \"0x%x\")(text - \"%s\")!\n", token->type, token_text(token));
    longjmp(__error_catch, 1);
}

void throw_invalid_ast_(char* __fun, char* __file, uint16_t __line, ast_expr_t* expression) {
    fprintf(stderr, "[%s, %s, %d]\n", __fun, __file, __line);
    fprintf(stderr, "Invalid AST (type - \"0x%x\")(address - \"0x%lx\")!\n", expression->type, (uintptr_t) expression);
    longjmp(__error_catch, 1);
}

void throw_unknown_error_(char* __fun, char* __file, uint16_t __line) {
    fprintf(stderr, "[%s, %s, %d]\n", __fun, __file, __line);
    fprintf(stderr, "Unknown Error!\n");
    longjmp(__error_catch, 1);
}