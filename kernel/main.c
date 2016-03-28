#include "mini64.h"

void
main()
{
    size_t i;
    uint16_t* vram = (void*)LOW_MEM(0xb8000);

    serial_write("Hello world");

    for (i = 0; i < 80*25; i++) {
        vram[i] = 0x3000;
    }
}
