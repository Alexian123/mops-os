[BITS 32]

; ensures this file gets linked before anything else (see makefile and linker script)
section .text

CODE_SEG equ 0x8
DATA_SEG equ 0x10

extern kmain

global _start
_start:
    ; setup segments and stack
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x00200000
    mov esp, ebp

    ; fast enable A20 gate
    in al, 0x92
    or al, 2
    out 0x92, al

    ; enter kernel
    call kmain

    ; should never reach this point, but who knows?
    cli
hang:
    hlt
    jmp hang

; pad to 512 bytes for 16 byte alignment
times 512-($-$$) db 0