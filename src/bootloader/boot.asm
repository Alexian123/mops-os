ORG 0x7C00
[BITS 16]

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

message:    db 'Loading kernel...', 0Dh, 0Ah, 0

[BITS 32]
pmode_start:
    mov eax, 1          ; starting sector LBA (bootloader=0)
    mov ecx, 100        ; total sectors to read
    mov edi, 0x0100000  ; target buffer (1MB)
    call ata_lba_read
    jmp CODE_SEG:0x0100000

ata_lba_read:
    mov ebx, eax        ; save LBA

    ; send byte 3 of LBA to HW controller
    shr eax, 24
    or eax, 0E0h        ; select master drive
    mov dx, 0x1F6       ; port address
    out dx, al

    ; send total sectors to HW controller
    mov eax, ecx
    mov dx, 0x1F2       ; port address
    out dx, al

    ; send byte 0 of LBA to HW controller
    mov eax, ebx
    mov dx, 0x1F3       ; port address
    out dx, al

    ; send byte 1 of LBA to HW controller
    mov eax, ebx
    shr eax, 8
    mov dx, 0x1F4       ; port address
    out dx, al

    ; send byte 2 of LBA to HW controller
    mov eax, ebx
    shr eax, 16
    mov dx, 0x1F5       ; port address
    out dx, al

    ; ???
    mov al, 20h
    mov dx, 0x1F7       ; port address
    out dx, al

    ; read sectors into memory
.next_sector:
    push ecx

.retry:
    ; check reading is needed
    mov dx, 0x1F7       ; port address
    in al, dx
    test al, 8
    jz .retry

    ; must read 256 words at a time
    mov ecx, 256
    mov dx, 0x1F0       ; port address
    rep insw

    ; read next sector
    pop ecx
    loop .next_sector

    ret

times 510-($-$$) db 0
dw 0AA55h