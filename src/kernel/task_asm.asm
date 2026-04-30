global switch_context

section .text
bits 64

; switch_context(void** old_rsp, void* new_rsp)
switch_context:
    ; Push callee-saved registers
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15

    ; Save current stack pointer
    mov [rdi], rsp

    ; Load next stack pointer
    mov rsp, rsi

    ; Pop callee-saved registers for the next task
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp

    ret
