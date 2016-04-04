use64

global start

extern main
extern paging_init
extern console_init

start:
    ; reload data segment registers
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; set up stack
    mov rsp, 0xffffff00001ffff8

    call console_init

    call paging_init

    call main

    cli
    hlt
