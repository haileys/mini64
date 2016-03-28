ENTRY(start)
OUTPUT_FORMAT(binary)

kernel_base = 0xffffff0000007e00;

SECTIONS {
    . = kernel_base;

    .text : AT(ADDR(.text) - kernel_base) {
        kernel/start.o (.text)
        * (.text)
    }

    .rodata ALIGN(0x1000) : AT(ADDR(.rodata) - kernel_base) {
        * (.rodata)
    }

    .data ALIGN(0x1000) : AT(ADDR(.data) - kernel_base) {
        * (.data)
    }
}
