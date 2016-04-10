#include "mini64.h"
#include "gdt.h"
#include "virt.h"
#include "console.h"
#include "string.h"
#include "panic.h"

#define GDT_DESCRIPTOR (1ull << 44)
#define GDT_PRESENT    (1ull << 47)
#define GDT_READWRITE  (1ull << 41)
#define GDT_EXECUTABLE (1ull << 43)
#define GDT_64BIT      (1ull << 53)

#define GDT_TSS        (9ull << 40)

typedef struct {
    uint32_t _reserved1;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t _reserved2;
    uint64_t ist[7];
    uint64_t _reserved3;
    uint16_t _reserved4;
    uint16_t iopb_base;
} __attribute__((packed))
tss_t;

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed))
gdtr_t;

typedef struct {
    gdtr_t gdtr;
    uint64_t entries[5];
    tss_t tss;
}
gdt_page_t;

void
gdt_init()
{
    gdt_page_t* p;
    if (virt_alloc((void*)&p) != OK) {
        panic("could not allocate GDT page");
    }

    memzero(p, sizeof(*p));

    p->gdtr.limit = sizeof(p->entries) - 1;
    p->gdtr.base = (uint64_t)&p->entries;

    p->entries[0] = 0;

    // code segment
    p->entries[SEL_CODE / 8] = GDT_DESCRIPTOR | GDT_PRESENT | GDT_READWRITE | GDT_EXECUTABLE | GDT_64BIT;

    // data segment
    p->entries[SEL_DATA / 8] = GDT_DESCRIPTOR | GDT_PRESENT | GDT_READWRITE;

    // tss segment - occupies two entries (16 bytes)
    uint64_t tss_base = (uint64_t)&p->tss;

    p->entries[SEL_TSS / 8] = 0xffffull                         // limit 0:15
                            | (tss_base & 0xfffful) << 16       // base 0:15
                            | ((tss_base >> 16) & 0xfful) << 32 // base 16:23
                            | GDT_PRESENT
                            | GDT_TSS
                            | 0xful << 48                       // limit 16:19
                            | ((tss_base >> 24) & 0xff) << 56   // base 24:31
                            ;
    p->entries[(SEL_TSS / 8) + 1] = (tss_base >> 32) & 0xfffffffful;

    // load new GDT
    __asm__ volatile("lgdt %0" :: "m"(p->gdtr) : "memory");
}
