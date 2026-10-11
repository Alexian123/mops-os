section .asm

; u8 ASMCALL inb(u16 port)
global inb
inb:
    push ebp
    mov ebp, esp

    xor eax, eax
    mov edx, [ebp + 8]
    in al, dx

    pop ebp
    ret

; u16 ASMCALL inw(u16 port)
global inw
inw:
    push ebp
    mov ebp, esp

    xor eax, eax
    mov edx, [ebp + 8]
    in ax, dx

    pop ebp
    ret

; void ASMCALL outb(u16 port, u8 data)
global outb
outb:
    push ebp
    mov ebp, esp

    mov edx, [ebp + 8]
    mov eax, [ebp + 12]
    out dx, al

    pop ebp
    ret

; void ASMCALL outw(u16 port, u16 data)
global outw
outw:
    push ebp
    mov ebp, esp

    mov edx, [ebp + 8]
    mov eax, [ebp + 12]
    out dx, ax

    pop ebp
    ret