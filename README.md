# mops-os

## Dependencies (Ubuntu/Debian packages) - WIP
- wget
- tar
- build-essential
- bison
- flex
- libgmp3-dev
- libmpc-dev
- libmpfr-dev
- texinfo
- libisl-dev
- nasm
- qemu-system-x86
- gdb

## Build & Run Instructions

- Build toolchain (GCC cross compiler & binutils):
```sh
make toolchain
```

- Build project
```sh
make    # defaults to 'make all'
```

- Build and run in QEMU
```sh
make run
```

- Clean project:
```sh
make clean
```

- Debug:
```sh
# in terminal 1:
make run_debug      # runs qemu
# in terminal 2:
make debug          # runs gdb
```