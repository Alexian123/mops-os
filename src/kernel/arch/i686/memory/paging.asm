section .asm

; void ASMCALL paging_load_directory(u32 *dir)
global paging_load_directory
paging_load_directory:
    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]
    mov cr3, eax

    pop ebp
    ret

; void ASMCALL paging_enable(void)
global paging_enable
paging_enable:
    push ebp
    mov ebp, esp

    mov eax, cr0
    or eax, 80000000h
    mov cr0, eax

    pop ebp
    ret