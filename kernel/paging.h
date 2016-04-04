#ifndef PAGING_H
#define PAGING_H

#include "mini64.h"
#include "error.h"

typedef uint64_t phys_t;
typedef uint64_t virt_t;

typedef uint64_t page_flags_t;

#define PAGE_SIZE 4096

#define PAGE_MASK 0x000ffffffffff000

#define PML4_INDEX(virt) ((((uint64_t)(virt)) >> 39) & 511)
#define PML3_INDEX(virt) ((((uint64_t)(virt)) >> 30) & 511)
#define PML2_INDEX(virt) ((((uint64_t)(virt)) >> 21) & 511)
#define PML1_INDEX(virt) ((((uint64_t)(virt)) >> 12) & 511)

#define PML4_TABLE       ((uint64_t*)(0xfffffffffffff000ull))
#define PML3_TABLE(virt) ((uint64_t*)(0xffffffffffe00000ull + ((((uint64_t)(virt)) >> 27) & 0x00001ff000ull)))
#define PML2_TABLE(virt) ((uint64_t*)(0xffffffffc0000000ull + ((((uint64_t)(virt)) >> 18) & 0x003ffff000ull)))
#define PML1_TABLE(virt) ((uint64_t*)(0xffffff8000000000ull + ((((uint64_t)(virt)) >> 9) & 0x7ffffff000ull)))

#define PML4_ENTRY(virt) PML4_TABLE[PML4_INDEX(virt)]
#define PML3_ENTRY(virt) PML3_TABLE(virt)[PML3_INDEX(virt)]
#define PML2_ENTRY(virt) PML2_TABLE(virt)[PML2_INDEX(virt)]
#define PML1_ENTRY(virt) PML1_TABLE(virt)[PML1_INDEX(virt)]

#define PAGE_PRESENT    (1ull << 0)
#define PAGE_WRITABLE   (1ull << 1)
#define PAGE_USER       (1ull << 2)
#define PAGE_WRITETHRU  (1ull << 3)
#define PAGE_NOCACHE    (1ull << 4)
#define PAGE_ACCESSED   (1ull << 5)
#define PAGE_DIRTY      (1ull << 6)
#define PAGE_HUGE       (1ull << 7)
#define PAGE_GLOBAL     (1ull << 8)
#define PAGE_NX         (1ull << 63)

void
paging_init();

error_t
page_map(virt_t virt, phys_t phys, page_flags_t flags);

void
page_unmap(virt_t virt);

error_t
phys_alloc(phys_t* out_phys);

error_t
phys_alloc_zero(phys_t* out_phys);

void
phys_free(phys_t phys);

#endif
