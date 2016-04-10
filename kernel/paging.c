#include "mini64.h"
#include "console.h"
#include "critical.h"
#include "paging.h"
#include "error.h"
#include "string.h"
#include "panic.h"

#define REGION_USABLE           1
#define REGION_RESERVED         2
#define REGION_ACPI_RECLAIMABLE 3
#define REGION_ACPI_NVS         4
#define REGION_BAD              5

struct memory_region {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t acpi_attributes;
};

struct memory_map {
    uint16_t count;
    struct memory_region regions[];
};

static void* const
temp_map_addr = (void*)0xffffff00001fd000;

static void
invlpg(void* addr)
{
    __asm__ volatile("invlpg %0" :: "m"(addr) : "memory");
}

uint64_t
paging_current_pml4()
{
    uint64_t pml4;
    __asm__ volatile("mov %%cr3, %0" : "=r"(pml4));
    return pml4;
}

void
paging_set_pml4(uint64_t pml4)
{
    __asm__ volatile("mov %0, %%cr3" :: "r"(pml4) : "memory");
}

static void*
map_temp(phys_t phys)
{
    PML1_ENTRY(temp_map_addr) = phys | PAGE_PRESENT | PAGE_WRITABLE;

    invlpg(temp_map_addr);

    return temp_map_addr;
}

static void
unmap_temp()
{
    PML1_ENTRY(temp_map_addr) = 0;

    invlpg(temp_map_addr);
}

void
paging_init()
{
    // the bootloader identity maps the first 2 MiB of physical memory
    // since that mapping is the only thing in the lower half of the address
    // space at this stage, it's safe to just remove PML4[0]
    PML4_TABLE[0] = 0;

    // map the first 2 MiB of physical memory back in at LOW_MEM_MAPPING_BASE
    PML2_ENTRY(LOW_MEM_MAPPING_BASE) = 0 | PAGE_PRESENT | PAGE_WRITABLE | PAGE_HUGE;

    // force a TLB flush
    paging_set_pml4(paging_current_pml4());
}

void
phys_init()
{
    // provided to us by the bootloader. this magic address only works until
    // we remove the identity map of the first 2 MiB of RAM
    const struct memory_map* memory_map = LOW_MEM(0x8000);

    log("paging_init");

    size_t usable_pages = 0;

    for (size_t i = 0; i < memory_map->count; i++) {
        const struct memory_region* region = &memory_map->regions[i];

        if (region->type != REGION_USABLE) {
            continue;
        }

        log("  found usable region!");

        phys_t base = region->base;
        phys_t end = region->base + region->length;

        if (base < 0x100000) {
            // don't use physical memory below the 1 MiB mark
            base = 0x100000;
        }

        if (base % PAGE_SIZE) {
            // round up base to the next page boundary
            base += PAGE_SIZE - (base % PAGE_SIZE);
        }

        if (end % PAGE_SIZE) {
            // round down end to the previous page boundary
            end -= (end % PAGE_SIZE);
        }

        log_x64("    base", base);
        log_x64("    end", end);

        for (phys_t phys = base; phys < end; phys += PAGE_SIZE) {
            phys_free(phys);

            usable_pages++;
        }
    }

    log_x64("usable memory pages", usable_pages);
}

static error_t
pm_entry_alloc(uint64_t* out_pm_entry)
{
    phys_t phys;
    error_t rc = phys_alloc_zero(&phys);

    if (rc != OK) {
        return rc;
    }

    *out_pm_entry = phys | PAGE_PRESENT | PAGE_WRITABLE;

    return OK;
}

error_t
page_map(virt_t virt, phys_t phys, page_flags_t flags)
{
    error_t rc;

    BEGIN_CRITICAL_SECTION;

    if (!(PML4_ENTRY(virt) & PAGE_PRESENT)) {
        uint64_t pm_entry;
        CHECKED(pm_entry_alloc(&pm_entry));

        PML4_ENTRY(virt) = pm_entry;

        invlpg(PML3_TABLE(virt));
    }

    if (!(PML3_ENTRY(virt) & PAGE_PRESENT)) {
        uint64_t pm_entry;
        CHECKED(pm_entry_alloc(&pm_entry));

        PML3_ENTRY(virt) = pm_entry;

        invlpg(PML2_TABLE(virt));
    }

    if (!(PML2_ENTRY(virt) & PAGE_PRESENT)) {
        uint64_t pm_entry;
        CHECKED(pm_entry_alloc(&pm_entry));

        PML2_ENTRY(virt) = pm_entry;

        invlpg(PML1_TABLE(virt));
    }

    if (PML1_ENTRY(virt) & PAGE_PRESENT) {
        RETURN(EALREADY);
    }

    PML1_ENTRY(virt) = (phys & PAGE_MASK) | flags;
    invlpg((void*)virt);

    RETURN(OK);

out:
    END_CRITICAL_SECTION;

    return rc;
}

void
page_unmap(virt_t virt)
{
    if (!(PML4_TABLE[PML4_INDEX(virt)] & PAGE_PRESENT)) {
        goto fail;
    }

    if (!(PML3_TABLE(virt)[PML3_INDEX(virt)] & PAGE_PRESENT)) {
        goto fail;
    }

    if (!(PML2_TABLE(virt)[PML2_INDEX(virt)] & PAGE_PRESENT)) {
        goto fail;
    }

    if (!(PML1_ENTRY(virt) & PAGE_PRESENT)) {
        goto fail;
    }

    PML1_ENTRY(virt) = 0;

    return;

fail:
    panic("tried to unmap unmapped address");
}

static phys_t
next_free_phys_page = 0;

error_t
phys_alloc(phys_t* out_phys)
{
    BEGIN_CRITICAL_SECTION;

    phys_t page = next_free_phys_page;

    if (page == 0) {
        return ENOMEM;
    }

    phys_t* temp_mapping = map_temp(page);

    next_free_phys_page = *temp_mapping;

    unmap_temp();

    END_CRITICAL_SECTION;

    *out_phys = page;

    return OK;
}

error_t
phys_alloc_zero(phys_t* out_phys)
{
    error_t rc;

    phys_t page;

    rc = phys_alloc(&page);

    if (rc != OK) {
        return rc;
    }

    BEGIN_CRITICAL_SECTION;

    uint64_t* ptr = map_temp(page);

    memzero(ptr, PAGE_SIZE);

    unmap_temp();

    END_CRITICAL_SECTION;

    *out_phys = page;

    return OK;
}

void
phys_free(phys_t phys)
{
    BEGIN_CRITICAL_SECTION;

    phys_t* temp_mapping = map_temp(phys);

    *temp_mapping = next_free_phys_page;

    unmap_temp();

    next_free_phys_page = phys;

    END_CRITICAL_SECTION;
}
