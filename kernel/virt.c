#include "virt.h"

#include "critical.h"
#include "paging.h"
#include "console.h"

typedef union free_page {
    union free_page* next;
    char dummy[PAGE_SIZE];
}
free_page_t;

static free_page_t*
next_free_page = NULL;

static free_page_t*
next_virtual_address = (void*)0xffffff0010000000;

static const free_page_t* const
max_virtual_address = (void*)0xffffff0020000000;

error_t
virt_alloc(void** out_page)
{
    error_t rc;

    BEGIN_CRITICAL_SECTION;

    if (next_free_page) {
        *out_page = next_free_page;

        next_free_page = next_free_page->next;

        RETURN(OK);
    }

    // if no free pages are in the free list, allocate another physical page
    // and map it in at next_virtual_address

    if (next_virtual_address == max_virtual_address) {
        RETURN(ENOMEM);
    }

    free_page_t* page = next_virtual_address++;

    phys_t phys;

    CHECKED(phys_alloc(&phys));

    CHECKED(page_map((virt_t)page, phys, PAGE_PRESENT | PAGE_WRITABLE));

    *out_page = page;
    RETURN(OK);

out:
    END_CRITICAL_SECTION;
    return rc;
}

void
virt_free(void* page)
{
    free_page_t* free_page = page;

    BEGIN_CRITICAL_SECTION;

    free_page->next = next_free_page;

    next_free_page = free_page;

    END_CRITICAL_SECTION;
}
