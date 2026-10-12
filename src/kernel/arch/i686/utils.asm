section .asm

; void ASMCALL disable_interrupts(void)
global disable_interrupts
disable_interrupts:
    cli
    ret

; void ASMCALL enable_interrupts(void)
global enable_interrupts
enable_interrupts:
    sti
    ret

; void ASMCALL hang(void)
global hang
hang:
    cli
.trap
    hlt
    jmp .trap
    ret ; how did I get here?