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
