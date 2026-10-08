target remote localhost:1234

break *0x7c00
break *0x100000

define asm
    x/10i $pc
end