#ifndef MINI64_H
#define MINI64_H

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef uint64_t size_t;

typedef uint64_t phys_t;

#define LOW_MEM(addr) (0xffffff0000000000ull | ((addr) & 0xfffff))

void
serial_write(const char* str);

#endif
