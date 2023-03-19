#include <util.h>
#include <stdlib.h>
#include <string.h>

void** util_reallocadd(void** arr, void* expression, size_t nsize) {
    size_t size = sizeof(void*) * nsize;
    void** tmp = malloc(size);
    memcpy(tmp, arr, size - sizeof(void*));
    free(arr);
    tmp[nsize - 1] = expression;
    return tmp;
}

bool util_token_cmpfree(token_t* token, const char* cmp) {
    return util_cmpfree(token_text(token), cmp);
}

bool util_cmpfree(char* str, const char* cmp) {
    bool result = !strcmp(str, cmp);
    free(str);
    return result;
}
