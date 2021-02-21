#!/bin/sh
bcc -ansi -c -o kernel.o kernel.c
nasm -f as86 kernel.asm -o kernel_asm.o
nasm -f as86 external.asm -o external_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o external_asm.o
dd if=kernel of=system.img bs=512 conv=notrunc seek=1
echo "Success, Disk Image has been created"