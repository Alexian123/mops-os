target remote localhost:1234

add-symbol-file ./build/kernel/kernel.elf

break *0x7c00
break *0x100000
break kmain

define asm
    x/10i $pc
end