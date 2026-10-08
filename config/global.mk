# ! THIS FILE MUST BE INCLUDED AT THE VERY BEGINNING OF THE MAIN MAKEFILE !

# For host arch
export CFLAGS=-std=c99 -g
export ASMFLAGS=
export CC=gcc
export CXX=g++
export LD=gcc
export ASM=nasm
export LINKFLAGS=
export LIBS=

# For target arch
export TARGET=i686-elf
export TARGET_ASM=nasm
export TARGET_ASMFLAGS=-f elf -g
export TARGET_CFLAGS=-std=c99 -g -O2
export TARGET_CC=$(TARGET)-gcc
export TARGET_CXX=$(TARGET)-g++
export TARGET_LD=$(TARGET)-gcc
export TARGET_LINKFLAGS=-g
export TARGET_LIBS=

export PROJECT_ROOT_DIR=$(abspath .)
export SCRIPTS_DIR=$(PROJECT_ROOT_DIR)/scripts
export CONFIG_DIR=$(PROJECT_ROOT_DIR)/config
export TOOLCHAIN_DIR=$(PROJECT_ROOT_DIR)/toolchain
export BUILD_DIR=$(PROJECT_ROOT_DIR)/build
export SRC_DIR=$(PROJECT_ROOT_DIR)/src
export BOOTLOADER_SRC_DIR=$(SRC_DIR)/bootloader
export KERNEL_SRC_DIR=$(SRC_DIR)/kernel
export KERNEL_INCLUDE_DIR=$(KERNEL_SRC_DIR)/include

# For make multithreaded make
export NUM_LOGICAL_CORES=`nproc`

# Cross toolchain
BINUTILS_VERSION = 2.37
BINUTILS_URL = https://ftp.gnu.org/gnu/binutils/binutils-$(BINUTILS_VERSION).tar.xz
GCC_VERSION = 11.2.0
GCC_URL = https://ftp.gnu.org/gnu/gcc/gcc-$(GCC_VERSION)/gcc-$(GCC_VERSION).tar.xz