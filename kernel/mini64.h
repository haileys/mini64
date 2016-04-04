#ifndef MINI64_H
#define MINI64_H

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef uint64_t size_t;

#define LOW_MEM(addr) ((void*)(addr))

#define NULL ((void*)0)

#endif
