#include "console.h"
#include "idt.h"
#include "io.h"
#include "virt.h"
#include "string.h"
#include "panic.h"

#define IDT_INTERRUPT_GATE 0x8e
#define IDT_TRAP_GATE      0x8f

typedef struct {
    uint16_t offset_0_15;
    uint16_t segment;
    uint8_t ist;
    uint8_t type;
    uint16_t offset_16_31;
    uint32_t offset_32_63;
    uint32_t _reserved;
} __attribute__((packed))
idt_entry_t;

typedef struct {
    uint16_t limit;
    idt_entry_t* base;
} __attribute__((packed))
idtr_t;

static void
remap_irqs()
{
    // remap IRQ table
    outb(0x20, 0x11);
    outb(0xa0, 0x11);
    outb(0x21, 0x20);
    outb(0xa1, 0x28);
    outb(0x21, 0x04);
    outb(0xa1, 0x02);
    outb(0x21, 0x01);
    outb(0xa1, 0x01);
    outb(0x21, 0x00);
    outb(0xa1, 0x00);
}

static void
set_isr(idt_entry_t* entry, uint64_t handler)
{
    entry->offset_0_15 = handler;
    entry->segment = 0x08;
    entry->ist = 0;
    entry->type = IDT_INTERRUPT_GATE;
    entry->offset_16_31 = handler >> 16;
    entry->offset_32_63 = handler >> 32;
    entry->_reserved = 0;
}

void
idt_init()
{
    error_t rc;

    remap_irqs();

    idt_entry_t* idt;
    if ((rc = virt_alloc((void**)&idt)) != OK) {
        log_x64("virt_alloc EALREADY?", rc == EALREADY);
        panic("could not allocate IDT");
    }

    memzero(idt, sizeof(idt_entry_t) * 256);

    void isr_32();
    set_isr(&idt[32], (uint64_t)&isr_32);

    void isr_39();
    set_isr(&idt[39], (uint64_t)&isr_39);

    idtr_t idtr = { .base = idt, .limit = sizeof(idt_entry_t) * 256 - 1 };

    __asm__ volatile("lidt %0" :: "m"(idtr));
}
