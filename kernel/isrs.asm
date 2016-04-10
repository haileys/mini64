use64

global idt_register_isrs

extern idt_set_interrupt_gate
extern panic
extern console_write

%macro begin_isr 1
    mov rdi, %1
    mov rsi, isr_%1
    call idt_set_interrupt_gate
    jmp isr_%1.end
    isr_%1:
%endmacro

%macro end_isr 0
    iretq
    .end:
%endmacro

%macro generic_exception 2
    begin_isr %1
        cli
        mov rdi, .msg
        call panic
        .msg db %2, 0
    end_isr
%endmacro

idt_register_isrs:

    generic_exception 0,  "divide by zero"
    generic_exception 1,  "debug"
    generic_exception 2,  "non-maskable interrupt"
    generic_exception 3,  "breakpoint"
    generic_exception 4,  "overflow"
    generic_exception 5,  "bound range exceeded"
    generic_exception 6,  "invalid opcode"
    generic_exception 7,  "device not available"
    generic_exception 8,  "double fault"
    generic_exception 10, "invalid tss"
    generic_exception 11, "segment not present"
    generic_exception 12, "stack segment fault"
    generic_exception 13, "general protection fault"
    generic_exception 14, "page fault"

    ; IRQ 0 - PIT
    begin_isr 32
        push rax
        mov al, 0x20
        out 0xa0, al
        out 0x20, al
        pop rax
    end_isr

    ; IRQ 1 - keyboard
    begin_isr 33
        push rax,
        mov al, 0x20
        out 0xa0, al
        out 0x20, al
        pop rax
    end_isr

    ; IRQ 7 - spurious
    begin_isr 39
    end_isr

ret
