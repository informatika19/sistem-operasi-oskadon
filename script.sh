#!/bin/sh
# Inisialisasi bootloader
dd if=/dev/zero of=system.img bs=512 count=2880
nasm bootloader.asm -o bootloader
dd if=bootloader of=system.img bs=512 count=1 conv=notrunc
dd if=image.img of=system.img bs=512 count=128 seek=1024 conv=notrunc

# Inisialisasi file system
dd if=map.img of=system.img bs=512 count=1 seek=256 conv=notrunc
dd if=files.img of=system.img bs=512 count=2 seek=257 conv=notrunc
dd if=sectors.img of=system.img bs=512 count=1 seek=259 conv=notrunc

# Inisialiasi kernel
bcc -ansi -c -o kernel.o kernel.c
bcc -ansi -c -o shell.o shell.c
nasm -f as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o fileIO.o folderIO.o math.o sector.o text.o
dd if=kernel of=system.img bs=512 conv=notrunc seek=1
echo "Success, Disk Image has been created"

# Compile loadFile
gcc loadFile.c -o loadFile

# Compile dan load shell
bcc -ansi -c -o shell.o shell.c
nasm -f as86 lib.asm -o lib_asm.o
ld86 -o shell -d shell.o lib_asm.o fileIO.o folderIO.o math.o sector.o text.o
./loadFile shell

# File txt external
./loadFile tes.txt

# Aplikasi
bcc -ansi -c -o cek.o cek.c
ld86 -o cek -d cek.o lib_asm.o fileIO.o math.o sector.o text.o
./loadFile cek

# Aplikasi
bcc -ansi -c -o mv.o mv.c
ld86 -o mv -d mv.o lib_asm.o fileIO.o math.o sector.o text.o
./loadFile mv
