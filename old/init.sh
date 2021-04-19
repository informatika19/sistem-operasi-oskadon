#!/bin/sh
dd if=/dev/zero of=system.img bs=512 count=2880
nasm bootloader.asm -o bootloader
dd if=bootloader of=system.img bs=512 count=1 conv=notrunc
dd if=image.img of=system.img bs=512 count=128 seek=1024 conv=notrunc