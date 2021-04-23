#!/bin/sh
bcc -ansi -c -o ../fileIO.o fileIO.c
bcc -ansi -c -o ../folderIO.o folderIO.c
bcc -ansi -c -o ../math.o math.c
bcc -ansi -c -o ../sector.o sector.c
bcc -ansi -c -o ../text.o text.c
echo "Compile Success!"