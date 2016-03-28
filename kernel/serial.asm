; very basic serial driver that only supports writing to COM1

global serial_write

COM1 equ 0x3f8
REG_LINE_STAT equ 5

; void serial_write_byte(char c)
serial_write_byte:
    mov dx, COM1 + REG_LINE_STAT
.ready_loop:
    in al, dx
    bt ax, 5
    jnc .ready_loop

    mov dx, COM1
    mov rax, rdi
    out dx, al

    ret

; void serial_write(const char* str)
serial_write:
    mov rsi, rdi
.loop:
    lodsb
    or al, al
    jz .done
    mov rdi, rax
    call serial_write_byte
    jmp .loop
.done:
    ret
