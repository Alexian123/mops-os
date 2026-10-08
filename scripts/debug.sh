#!/bin/bash

qemu-system-x86_64 \
    -hda $2 \
    -S \
    -gdb tcp::1234 &

QEMU_PID=$!

gdb -x $1

kill $QEMU_PID