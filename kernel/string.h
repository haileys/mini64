#ifndef STRING_H
#define STRING_H

#include "stddef.h"

void
memmove(void* dst, const void* src, size_t n);

void
memzero(void* ptr, size_t size);

#endif
