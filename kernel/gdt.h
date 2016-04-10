#ifndef GDT_H
#define GDT_H

#define SEL_CODE 0x08
#define SEL_DATA 0x10
#define SEL_TSS  0x18

void
gdt_init();

#endif
