#ifndef __UTIL_H__
#define __UTIL_H__

#include <lexer.h>

#include <stddef.h>
#include <stdbool.h>

void** util_reallocadd(void** arr, void* expression, size_t nsize);
bool util_token_cmpfree(token_t* token, const char* cmp);
bool util_cmpfree(char* str, const char* cmp);

#endif /* __UTIL_H__ */