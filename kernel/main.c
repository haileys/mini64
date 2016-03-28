typedef unsigned long long size_t;
typedef unsigned short uint16_t;

void
main()
{
    size_t i;
    uint16_t* vram = (void*)0xffffff00000b8000;

    for (i = 0; i < 80*25; i++) {
        vram[i] = 0x3000;
    }
}
