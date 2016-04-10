global panic
extern console_write
extern log
extern log_x64

panic:
    cli

    push rdi
    mov rdi, .panic
    call console_write
    pop rdi
    call log

.backtrace_loop:
    xor rdi, rdi
    mov rsi, [rsp]
    call log_x64

    test rbp, rbp
    jz .halt
    leave
    jmp .backtrace_loop
.halt:
    hlt

    .panic db "panic: ", 0
