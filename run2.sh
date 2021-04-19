#!/bin/bash
# Create a bootloader
nasm bootloader.asm -o bootloader
dd if=bootloader of=system.img bs=512 count=1 conv=notrunc

# Create system.img
dd if=/dev/zero of=system.img bs=512 count=2880

# Create map outside the script so the hex wont change
# dd if=/dev/zero of=map.img bs=512 count=1
# hexedit map.img (0xFF for 16 bit)

# Create files, and sectors.img
dd if=/dev/zero of=sectors.img bs=512 count=1
dd if=/dev/zero of=files.img bs=512 count=2

# Create a bootloader
nasm bootloader.asm -o bootloader
dd if=bootloader of=system.img bs=512 count=1 conv=notrunc

# Insert map,file, and sectors.img to system.img
dd if=map.img of=system.img bs=512 count=1 seek=256 conv=notrunc
dd if=files.img of=system.img bs=512 count=2 seek=257 conv=notrunc
dd if=sectors.img of=system.img bs=512 count=1 seek=259 conv=notrunc
dd if=image.img of=system.img bs=512 count=128 seek=1024 conv=notrunc

# compile kernel.c
bcc -ansi -c -o kernel.o kernel.c
nasm -f as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o fileIO.o folderIO.o math.o text.o
dd if=kernel of=system.img bs=512 conv=notrunc seek=1

# compile loadfile
gcc loadFile.c -o loadFile

# compile shell
bcc -ansi -c -o shell.o shell.c
nasm -f as86 lib.asm -o lib_asm.o
ld86 -o shell -d shell.o lib_asm.o 
./loadFile shell


# test load file
./loadFile coba.txt
