#include "console.h"
#include "idt.h"
#include "io.h"
#include "virt.h"
#include "string.h"
#include "panic.h"
#include "gdt.h"

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

static idt_entry_t*
idt;

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

void
idt_set_interrupt_gate(uint8_t vector, uint64_t handler)
{
    idt[vector].offset_0_15 = handler;
    idt[vector].segment = SEL_CODE;
    idt[vector].ist = 0;
    idt[vector].type = IDT_INTERRUPT_GATE;
    idt[vector].offset_16_31 = handler >> 16;
    idt[vector].offset_32_63 = handler >> 32;
    idt[vector]._reserved = 0;
}

void
idt_register_isrs();

void
idt_init()
{
    error_t rc;

    remap_irqs();

    if ((rc = virt_alloc((void**)&idt)) != OK) {
        log_x64("virt_alloc EALREADY?", rc == EALREADY);
        panic("could not allocate IDT");
    }

    memzero(idt, sizeof(idt_entry_t) * 256);

    idt_register_isrs();

    idtr_t idtr = { .base = idt, .limit = sizeof(idt_entry_t) * 256 - 1 };

    __asm__ volatile("lidt %0" :: "m"(idtr));
}
