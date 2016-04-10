#ifndef IO_H
#define IO_H

#include "stddef.h"

static inline void
outb(uint16_t port, uint8_t value)
{
    __asm__ volatile("outb %0, %1" :: "r"(value), "Nd"(port));
}

#endif
