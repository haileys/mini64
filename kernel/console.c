#include "console.h"
#include "string.h"
#include "io.h"

#define CONSOLE_COLUMNS 80
#define CONSOLE_ROWS 25

typedef struct {
    char c;
    uint8_t attr;
}
vchar_t;

static size_t current_row = 0, current_column = 0;

static uint16_t base_vga_port;

static vchar_t* const vram = LOW_MEM(0xb8000);

static void
console_putc(char c)
{
    if (c == '\n') {
        current_row++;
        current_column = 0;
    } else {
        size_t index = current_row * CONSOLE_COLUMNS + current_column;

        vram[index].c = c;
        vram[index].attr = 0x0f;

        current_column++;

        if (current_column == CONSOLE_COLUMNS) {
            current_row++;
            current_column = 0;
        }
    }

    if (current_row == CONSOLE_ROWS) {
        current_row--;
        memmove(vram, vram + CONSOLE_COLUMNS, CONSOLE_COLUMNS * (CONSOLE_ROWS - 1) * 2);
        memzero64((uint64_t*)(vram + CONSOLE_COLUMNS * (CONSOLE_ROWS - 1)), (CONSOLE_COLUMNS * 2) / 8);
    }
}

static void
update_cursor()
{
    size_t position = current_row * CONSOLE_COLUMNS + current_column;

    outb(base_vga_port, 0x0e);
    outb(base_vga_port + 1, (position >> 8) & 0xff);

    outb(base_vga_port, 0x0f);
    outb(base_vga_port + 1, position & 0xff);
}

void
console_init()
{
    // read base vga port from bios data area
    base_vga_port = *(uint16_t*)LOW_MEM(0x463);
}

void
console_write(const char* str)
{
    for (; *str; str++) {
        console_putc(*str);
    }

    update_cursor();
}

void
log(const char* msg)
{
    console_write(msg);
    console_write("\n");
}

void
log_x64(const char* msg, uint64_t u64)
{
    console_write(msg);
    console_write(": 0x");

    static const char map[] = "0123456789abcdef";

    char buff[17] = { 0 };

    for (int i = 0; i < 16; i++) {
        buff[i] = map[(u64 >> (60 - i * 4)) & 0xf];
    }

    console_write(buff);
    console_write("\n");
}

void
panic(const char* msg)
{
    console_write("panic: ");
    console_write(msg);
    console_write("\n");
    __asm__ volatile ("cli \n hlt");
}
