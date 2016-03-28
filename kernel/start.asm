use64

global start
extern main

pml4 equ 0xfffffffffffff000

start:
    ; reload data segment registers
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; set up stack
    mov rsp, 0xffffff0000007bfc

    ; unmap identity mapped first 2MB
    mov qword [pml4], 0

    ; reload cr3 to flush TLB
    mov rax, cr3
    mov cr3, rax

    call main

    cli
    hlt
