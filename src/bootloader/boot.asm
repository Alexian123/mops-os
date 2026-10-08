ORG 0x7C00
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; short jump + nop (3 bytes) required before BPB
jmp short init_cs
nop

; buffer for BPB
times 33 db 0

init_cs:
    jmp 0x0000:start    ; set CS = 0x000 

start:
    ; set up segments and stack
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    ; print message
    cld
    mov si, message
    call print

    ; load GTD
    cli
    lgdt [gdt_descriptor]

    ; enter pmode
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp CODE_SEG:pmode_start

hang:
    cli
    hlt
    jmp $

; DS:SI = null terminated char*
print:
    mov ah, 0Eh
    mov bx, 0
.loopc:
    lodsb
    cmp al, 0
    je .done
    int 10h
    jmp .loopc
.done:
    ret

; GDT
gdt_start:
    ; null descriptor
    dd 0
    dd 0    
gdt_code:   ; offset 0x8
    dw 0xffff       ; Segment limit first 0-15 bits
    dw 0            ; Base first 0-15 bits
    db 0            ; Base 16-23 bits
    db 0x9a         ; Access byte
    db 11001111b    ; High 4 bit flags and the low 4 bit flags
    db 0            ; Base 24-31 bits
gdt_data:   ; offset 0x10
    dw 0xffff       ; Segment limit first 0-15 bits
    dw 0            ; Base first 0-15 bits
    db 0            ; Base 16-23 bits
    db 0x92         ; Access byte
    db 11001111b    ; High 4 bit flags and the low 4 bit flags
    db 0            ; Base 24-31 bits
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

message:    db 'Hello World!', 0Dh, 0Ah, 0
msg_error:  db 'ERROR: Failed to read disk', 0Dh, 0Ah, 0

[BITS 32]
pmode_start:
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

times 510-($-$$) db 0
dw 0AA55h