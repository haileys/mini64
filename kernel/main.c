#include "stddef.h"
#include "paging.h"
#include "console.h"
#include "virt.h"
#include "panic.h"

void
main()
{
    log("Hello world");

    void* page;
    if (virt_alloc(&page) != OK) {
        panic("could not allocate page");
    }

    log_x64("allocated", (uint64_t)page);
}
