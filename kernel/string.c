#include "string.h"

void
memmove(void* dst_, const void* src_, size_t n)
{
    char* dst = dst_;
    const char* src = src_;

    if (dst < src) {
        for (size_t i = 0; i < n; i++) {
            dst[i] = src[i];
        }
    } else {
        for (size_t i = 0; i < n; i++) {
            dst[n - i - 1] = src[n - i - 1];
        }
    }
}

void
memzero64(uint64_t* ptr, size_t count)
{
    for (size_t i = 0; i < count; i++) {
        ptr[i] = 0;
    }
}
