global gdt_load
global tss_load

section .text
bits 64

gdt_load:
    lgdt [rdi]
    ; Reload segments
    mov ax, 0x10 ; Kernel Data (2nd entry, 0x10)
    mov ds, ax
    mov es, ax
    mov ss, ax
    
    ; Far return to reload CS
    pop rdi ; Return address
    mov rax, 0x08 ; Kernel Code (1st entry, 0x08)
    push rax
    push rdi
    retfq

tss_load:
    mov ax, 0x28 ; TSS entry is at 5 * 8 = 0x28
    ltr ax
    ret
