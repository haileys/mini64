use64

global start

extern main
extern paging_init
extern console_init
extern phys_init
extern idt_init

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
    xor rbp, rbp

    ; must be called before *anything* else
    call paging_init

    call console_init

    call phys_init

    call idt_init

    sti
.idle:
    hlt
    jmp .idle
