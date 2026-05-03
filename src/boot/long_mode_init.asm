global long_mode_start
extern kmain

section .text
bits 64
long_mode_start:
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; VGA 'OK' in light gray on black
    mov rax, 0x0f4b0f4f
    mov [0xb8000], rax

    call kmain
    hlt
