#ifndef STRING_H
#define STRING_H

#include "mini64.h"

void
memmove(void* dst, const void* src, size_t n);

void
memzero64(uint64_t* ptr, size_t count);

#endif
