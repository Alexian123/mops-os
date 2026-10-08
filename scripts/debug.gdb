
target remote localhost:1234

break *0x7c00
continue

display/i $pc
display/x $ax
display/x $bx
display/x $cx
display/x $dx
display/x $sp
display/x $ss
display/x $cs

x/16hx $sp