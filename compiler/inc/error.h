#ifndef __ERROR_H__
#define __ERROR_H__

#include <token.h>

#include <setjmp.h>

/* PUBLIC */

#define THROW_INFO (void*) __FUNCTION__, (void*) __FILE__, __LINE__

void catch_init();
#define throw_invalid_token(token) throw_invalid_token_(THROW_INFO, token);

/* THROWS */

void throw_invalid_token_(char* __fun, char* __file, uint16_t __line, token_t* token) __attribute__((__noreturn__));

#endif