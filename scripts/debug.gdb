target remote localhost:1234

break *0x7c00
continue

define asm
    x/10i $pc
end