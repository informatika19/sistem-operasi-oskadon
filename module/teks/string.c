#include "string.h"
#include "../math/math.h"

int strlen(char* buff) {
    int i = 0;
    while (buff[i] != '\0') {
        i++;
    }
    return i;
}

int strcmp(char* a, char* b) {
    int i;

    if (strlen(a) != strlen(b)) {
        return 0;
    }

    i = 0;
    while (i < strlen(a)) {
        if(a[i] != b[i]) {
            return 0;
        }
        i++;
    }

    return 1;
}

void strcpy(char* dest, char* src) {
    int i;

    i = 0;
    for (i; i < strlen(src); i++) {
        dest[i] = src[i];
    }

    src[i] = 0x00;
    dest[i] = 0x00;
}

void clear(char* buffer, int length) {
    int i = 0;
    while (i < length) {
        buffer[i] = 0x0;
    }
}

void printString(char* string) {
    int AL = 0x0E00;
    int i = 0;
    while (string[i] != '\0') {
        interrupt(0x10, AL+string[i], 0x000F, 0, 0);

        if (string[i] == '\n') {
            interrupt(0x10, AL+0xd, 0x000F, 0, 0);
        }

        i++;
    }
}

void readString(char* string) {
    int AL = 0x0E00;
    int loop = 1;
    int i = 0;
    int ascii;

    while (loop) {
        ascii = interrupt(0x16, 0, 0, 0, 0);
        if (ascii == 0xd) {
            string[i] = 0x0;
            interrupt(0x10, AL+0xa, 0, 0, 0);
            interrupt(0x10, AL+0xd, 0, 0, 0);
            loop = 0;
        }
        else if (ascii = 0x8) {
            if (i > 0 && mod(i, 80) != 0) {
                interrupt(0x10, AL+0x8, 0, 0, 0);
                interrupt(0x10, AL+0x0, 0, 0, 0);
                interrupt(0x10, AL+0x8, 0, 0, 0);
                i--;
            }
        }
        else {
            string[i] = ascii;
            interrupt(0x10, AL+ascii, 0, 0, 0);
            i++;
        }
    }
}