global start
extern long_mode_start

section .text
bits 32
start:
    mov esp, stack_top
    mov esi, ebx ; Save multiboot info pointer in esi

    ; Check multiboot
    cmp eax, 0x36d76289
    jne .no_multiboot

    call check_cpuid
    call check_long_mode

    call setup_page_tables
    call enable_paging

    ; Load the 64-bit GDT
    lgdt [gdt64.pointer]
    
    ; Pass multiboot info pointer in edi (first argument in x86_64 calling convention)
    mov edi, esi
    jmp gdt64.code:long_mode_start

    hlt

.no_multiboot:
    mov al, "M"
    jmp error

error:
    ; Print "ERR: X" where X is the error code in AL
    mov dword [0xb8000], 0x4f524f45
    mov dword [0xb8004], 0x4f3a4f52
    mov dword [0xb8008], 0x4f204f20
    mov byte  [0xb800a], al
    hlt

check_cpuid:
    ; Check if CPUID is supported by attempting to flip the ID bit (bit 21)
    ; in the FLAGS register. If we can flip it, CPUID is available.
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    cmp eax, ecx
    je .no_cpuid
    ret
.no_cpuid:
    mov al, "C"
    jmp error

check_long_mode:
    ; test if extended processor info is available
    mov eax, 0x80000000    ; implicit argument for cpuid
    cpuid                  ; get highest supported argument
    cmp eax, 0x80000001    ; it needs to be at least 0x80000001
    jb .no_long_mode       ; if it's less, the CPU is too old for long mode

    ; use extended info to test if long mode is available
    mov eax, 0x80000001    ; argument for extended processor info
    cpuid                  ; returns various feature bits in ecx and edx
    test edx, 1 << 29      ; test if the LM-bit is set in the edx register
    jz .no_long_mode       ; If it's not set, there is no long mode
    ret
.no_long_mode:
    mov al, "L"
    jmp error

setup_page_tables:
    ; Map first P4 entry to P3 table
    mov eax, p3_table
    or eax, 0b11 ; present + writable
    mov [p4_table], eax

    ; Map first 4 P3 entries to 4 P2 tables
    mov ecx, 0
.map_p3_table:
    mov eax, 4096
    mul ecx
    add eax, p2_table
    or eax, 0b11 ; present + writable
    mov [p3_table + ecx * 8], eax
    inc ecx
    cmp ecx, 4
    jne .map_p3_table

    ; Map each P2 entry to a huge 2MiB page (total 2048 entries = 4GiB)
    mov ecx, 0 ; counter
.map_p2_table:
    ; Map ecx-th P2 entry to a huge page that starts at address 2MiB*ecx
    mov eax, 0x200000  ; 2MiB
    mul ecx            ; start address of ecx-th page
    or eax, 0b10000011 ; present + writable + huge
    mov [p2_table + ecx * 8], eax ; map ecx-th entry

    inc ecx            ; increase counter
    cmp ecx, 2048      ; 2048 * 2MiB = 4GiB
    jne .map_p2_table  ; else map the next entry

    ret

enable_paging:
    ; load P4 to cr3 register (cpu uses this to access the P4 table)
    mov eax, p4_table
    mov cr3, eax

    ; enable PAE-flag in cr4 (Physical Address Extension)
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; set the long mode bit in the EFER MSR (model specific register)
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; enable paging in the cr0 register
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ret

section .bss
align 4096
p4_table:
    resb 4096
p3_table:
    resb 4096
p2_table:
    resb 4096 * 4
stack_bottom:
    resb 4096 * 4
stack_top:

section .rodata
gdt64:
    dq 0 ; zero entry
.code: equ $ - gdt64
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53) ; code segment
.pointer:
    dw $ - gdt64 - 1
    dq gdt64
