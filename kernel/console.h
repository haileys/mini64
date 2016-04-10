#ifndef CONSOLE_H
#define CONSOLE_H

#include "stddef.h"

void
console_write(const char* str);

void
log(const char* msg);

void
log_x64(const char* msg, uint64_t u64);

#endif
