use64

global isr_32
global isr_39

extern log_x64

pit_count dq 0

; IRQ 0 - PIT
isr_32:
    push rax
    push rbx

    mov rdi, .msg

    mov rbx, pit_count
    mov rsi, [rbx]
    inc rsi
    mov [rbx], rsi

    call log_x64

    mov al, 0x20
    out 0xa0, al
    out 0x20, al

    pop rbx
    pop rax

    iretq
.msg db "PIT", 0

; IRQ 7 - spurious
isr_39:
    iretq
