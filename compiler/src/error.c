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
    fprintf(stderr, "Invalid token (type - \"0x%x\")(text - \"%s\")\n", token->type, token_text(token));
    longjmp(__error_catch, 1);
}