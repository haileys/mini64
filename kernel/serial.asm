; very basic serial driver that only supports writing to COM1

global serial_init
global serial_write

COM1 equ 0x3f8

REG_INT_ENABLE equ 1
REG_FIFO_CTRL  equ 2
REG_LINE_CTRL  equ 3
REG_LINE_STAT  equ 5

REG_BAUD_LSB   equ 0
REG_BAUD_MSB   equ 1

BAUD_RATE equ 38400

; void serial_init(void)
serial_init:
    ; disable interrupts:

    mov dx, COM1 + REG_INT_ENABLE
    mov al, 0
    out dx, al

    ; set line baud:

    ; set DLAB
    mov dx, COM1 + REG_LINE_CTRL
    in al, dx
    mov ah, al ; save line control register in AH
    or al, 0x80
    out dx, al
    ; set LSB of baud divisor
    mov dx, COM1 + REG_BAUD_LSB
    mov al, (115200 / BAUD_RATE) & 0xff
    out dx, al
    ; set MSB of baud divisor
    mov dx, COM1 + REG_BAUD_LSB
    mov al, ((115200 / BAUD_RATE) >> 8) & 0xff
    out dx, al
    ; clear DLAB
    mov dx, COM1 + REG_LINE_CTRL
    mov al, ah ; restore line control register from before
    out dx, al

    ; set 8 data bits and 1 stop bit
    ; we have the contents of the line control register in AL already
    and al, 0xc0
    or al, 0x07
    out dx, al

    ; enable fifo, set buffer to 14 bytes:
    mov dx, COM1 + REG_FIFO_CTRL
    mov al, 0xc7
    out dx, al

    ret

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
