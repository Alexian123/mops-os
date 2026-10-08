[BITS 32]

CODE_SEG equ 0x8
DATA_SEG equ 0x10

global _start
_start:
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

    jmp $