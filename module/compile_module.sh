#!/bin/sh
bcc -ansi -c -o ../obj/fileIO.o fileIO.c
bcc -ansi -c -o ../obj/folderIO.o folderIO.c
bcc -ansi -c -o ../obj/math.o math.c
bcc -ansi -c -o ../obj/sector.o sector.c
bcc -ansi -c -o ../obj/text.o text.c
echo "Compile Success!"