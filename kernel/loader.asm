org 0x7c00
use16

    ; initialize cs
    jmp 0:start
start:
    ; initialize data segments
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    ; initialize stack
    mov sp, 0x7bfe
    ; load main kernel code into 0x7e00
    mov ah, 2       ; read
    mov al, 24      ; 24 sectors (12 KiB)
    mov ch, 0       ; cylinder & 0xff
    mov cl, 2       ; sector | ((cylinder >> 2) & 0xc0)
    mov dh, 0       ; head
    mov bx, 0x7e00  ; read buffer
    int 0x13
    mov si, could_not_read_disk
    jc error_bios
    ; enable A20 line
    mov ax, 0x2401
    int 0x15
    mov si, could_not_enable_a20
    jc error_bios
    ; load protected mode GDT and a null IDT (we don't need interrupts)
    cli
    lgdt [gdtr32]
    lidt [idtr32]
    ; set protected mode bit of cr0
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    ; far jump to load CS with 32 bit segment
    jmp 0x08:protected_mode

error_bios: ; pass msg in SI
.loop:
    lodsb
    or al, al
    jz .end
    mov ah, 0x0e
    int 0x10
    jmp .loop
.end:
    cli
    hlt

    use32

error: ; pass msg in ESI
    mov edi, 0xb8000
    mov ah, 0x4f ; white on red
.loop:
    lodsb
    or al, al
    jz .end
    stosw
    jmp .loop
.end:
    cli
    hlt

protected_mode:
    ; load all the other segments with 32 bit data segments
    mov eax, 0x10
    mov ds, eax
    mov es, eax
    mov ss, eax

    ; set up stack
    mov esp, 0x7bfc

    ; clear screen
    xor eax, eax
    mov edi, 0xb8000
    mov ecx, 80*25 / 2
    rep stosd

    ; check if extended processor information is supported by cpuid
    mov eax, 0x80000000
    cpuid
    mov esi, no_extended_processor_information
    cmp eax, 0x80000001
    jb error

    ; check if long mode is supported
    mov eax, 0x80000001
    cpuid
    mov esi, no_long_mode
    test edx, 1 << 29
    jz error

    ; zero out PML4, PDP and PD (assumes contiguity)
    xor eax, eax
    mov edi, pml4
    mov ecx, 0x3000 / 4
    rep stosd

    ; identity map first 2MiB of physical memory
    mov dword [pml4], pdp | PAGE_PRESENT | PAGE_WRITABLE
    mov dword [pdp], pd | PAGE_PRESENT | PAGE_WRITABLE
    mov dword [pd], 0x0 | PAGE_PRESENT | PAGE_WRITABLE | PAGE_HUGE

    ; load PML4 into CR3
    mov eax, pml4
    mov cr3, eax

    ; enable physical address extensions
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; enable long mode
    mov ecx, 0xc0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; enable paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; load 64 bit GDT
    lgdt [gdtr64]

    ; reload code segment
    jmp 0x08:long_mode

long_mode:
    use64 ; 🎉

    ; map kernel in higher half before jumping to it
    mov qword [pml4 + 510 * 8], pdp | PAGE_PRESENT | PAGE_WRITABLE

    ; recursively map PML4
    mov qword [pml4 + 511 * 8], pml4 | PAGE_PRESENT | PAGE_WRITABLE

    ; jump to into higher half kernel
    ; need to use an indirect jmp because relative jumps are only 32 bit
    mov rax, 0xffff_ff00_0000_7e00
    jmp rax

could_not_read_disk db "could not read disk", 0
could_not_enable_a20 db "could not enable A20", 0
no_extended_processor_information db "extended processor information not supported on this CPU", 0
no_long_mode db "long mode not supported on this CPU", 0

gdtr32:
    dw (gdt32.end - gdt32) - 1 ; size
    dd gdt32                   ; offset

idtr32:
    dw 0
    dd 0

gdt32:
    ; null entry
    dq 0
    ; code entry
    dw 0xffff       ; limit 0:15
    dw 0x0000       ; base 0:15
    db 0x00         ; base 16:23
    db 0b10011010   ; access byte - code
    db 0xcf         ; flags/(limit 16:19). 4 KB granularity + 32 bit mode flags
    db 0x00         ; base 24:31
    ; data entry
    dw 0xffff       ; limit 0:15
    dw 0x0000       ; base 0:15
    db 0x00         ; base 16:23
    db 0b10010010   ; access byte - data
    db 0xcf         ; flags/(limit 16:19). 4 KB granularity + 32 bit mode flags
    db 0x00         ; base 24:31
.end:

gdtr64:
    dw (gdt64.end - gdt64) - 1 ; size
    dd gdt64                   ; offset

gdt64:
    ; null entry
    dq 0
    ; code entry
    dq GDT64_DESCRIPTOR | GDT64_PRESENT | GDT64_READWRITE | GDT64_EXECUTABLE | GDT64_64BIT
    ; data entry
    dq GDT64_DESCRIPTOR | GDT64_PRESENT | GDT64_READWRITE
.end:

; GDT flags
GDT64_DESCRIPTOR equ 1 << 44
GDT64_PRESENT    equ 1 << 47
GDT64_READWRITE  equ 1 << 41
GDT64_EXECUTABLE equ 1 << 43
GDT64_64BIT      equ 1 << 53

; long mode page tables:
pml4 equ 0x1000
pdp  equ 0x2000
pd   equ 0x3000

; paging flags
PAGE_PRESENT  equ 1 << 0
PAGE_WRITABLE equ 1 << 1
PAGE_HUGE     equ 1 << 7

times 510-($-$$) db 0
db 0x55
db 0xaa
